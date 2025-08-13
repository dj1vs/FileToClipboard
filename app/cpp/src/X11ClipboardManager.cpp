#include "X11ClipboardManager.hpp"

// C++ STD headers
#include <stdexcept>
#include <array>
#include <algorithm>
#include <iostream>
#include <unordered_map>

// X11 headers
#include <X11/Xatom.h>

X11ClipboardManager::X11ClipboardManager(PayloadType payload_type, std::string payload)
: ClipboardManager(payload_type, payload)
{
    if (payload_type == PayloadType::File) this->payload = ClipboardManager::get_correct_file_payload_uri(payload);

    // basic X11 initialization
    dpy = XOpenDisplay(NULL);

    if (!dpy)
    {
        throw std::runtime_error("Can't open X Display");
    }

    int screen = DefaultScreen(dpy);

    Window root = RootWindow(dpy, screen);
    Window owner = XCreateSimpleWindow(dpy, root, -10, -10, 1, 1, 0, 0, 0);
    
    // claim selection ownership
    Atom sel = XInternAtom(dpy, "CLIPBOARD", False);

    XSetSelectionOwner(dpy, sel, owner, CurrentTime);
    if (XGetSelectionOwner(dpy, sel) != owner)
    {
        throw std::runtime_error("Failed to become selection owner\n");
    }
}

void X11ClipboardManager::run()
{
    

    std::vector<Atom> supported_arr;
    if (payload_type == PayloadType::File)
    {
        Atom target_file_type_atom = XInternAtom(dpy, get_file_mime_type().c_str(), False);
        Atom text_uri_list = XInternAtom(dpy, "text/uri-list", False);
        Atom gnome_file_data = XInternAtom(dpy, "x-special/gnome-copied-files", False);
        supported_arr = { target_file_type_atom, text_uri_list, gnome_file_data};
    }
    else
    {
        Atom utf8 = XInternAtom(dpy, "UTF8_STRING", False);
        Atom text = XInternAtom(dpy, "TEXT", False);
        Atom targets = XInternAtom(dpy, "TARGETS", False);
        Atom string = XInternAtom(dpy, "STRING", False);
        Atom text_plain = XInternAtom(dpy, "text/plain", False);
        Atom compound_text = XInternAtom(dpy, "COMPOUND_TEXT", False);
        supported_arr = { utf8, string, text, text_plain, compound_text };
    }

    XEvent x_event;
    while (true)
    {
        XNextEvent(dpy, &x_event);
        switch (x_event.type)
        {
            case SelectionClear:
                return;
            case SelectionRequest:
            {
                XSelectionRequestEvent* sev = (XSelectionRequestEvent*)&x_event.xselectionrequest;
                Atom targets = XInternAtom(dpy, "TARGETS", False);
            
                Atom requested = sev->target;
                char* an = XGetAtomName(dpy, requested);

                printf("Requestor: 0x%lx\n", sev->requestor);
                std::cout << "Requested target: " << (an ? an : "null") << std::endl;

                if (an) XFree(an);
            
                if (requested == targets)
                {
                    XChangeProperty(dpy, sev->requestor, sev->property, XA_ATOM, 32,
                                    PropModeReplace, reinterpret_cast<unsigned char*>(supported_arr.data()), static_cast<int>(supported_arr.size()));
                    
                    XSelectionEvent ssev;
                    ssev.type = SelectionNotify;
                    ssev.requestor = sev->requestor;
                    ssev.selection = sev->selection;
                    ssev.target = sev->target;
                    ssev.property = sev->property;
                    ssev.time = sev->time;
                        
                    XSendEvent(dpy, sev->requestor, True, NoEventMask, reinterpret_cast<XEvent *>(&ssev));
                }
                else if (sev->property != None 
                    && std::find(supported_arr.begin(), supported_arr.end(), requested) != supported_arr.end())
                {
                    send_msg(sev, requested);
                }
                else
                {
                    deny_request(sev);
                }
            
                break;
            }
        }
    }
}

void X11ClipboardManager::send_msg(XSelectionRequestEvent* sev, Atom target) {
    XChangeProperty(dpy, sev->requestor, sev->property, target, 8,
        PropModeReplace, reinterpret_cast<const unsigned char*>(payload.c_str()), static_cast<int>(payload.size()));

    XSelectionEvent ssev;
    ssev.type = SelectionNotify;
    ssev.requestor = sev->requestor;
    ssev.selection = sev->selection;
    ssev.target = target;
    ssev.property = sev->property;
    ssev.time = sev->time;

    XSendEvent(dpy, sev->requestor, True, NoEventMask, reinterpret_cast<XEvent*>(&ssev));
}


void X11ClipboardManager::deny_request(XSelectionRequestEvent* sev)
{
    XSelectionEvent ssev;

    ssev.type = SelectionNotify;
    ssev.requestor = sev->requestor;
    ssev.selection = sev->selection;
    ssev.target = sev->target;
    ssev.property = None;
    ssev.time = sev->time;

    XSendEvent(dpy, sev->requestor, True, NoEventMask, reinterpret_cast<XEvent *>(&ssev));
}

std::string get_cmd_output(const std::string &cmd);

std::string X11ClipboardManager::get_file_mime_type()
{
    // try to get mime type from file utility
    if (system("which file > /dev/null 2>&1") == 0)
    {
        std::string mime = get_cmd_output("file --mime-type -b \"" + payload + "\"");
        if (!mime.empty())
        {
            return mime;
        }
    }

    // if something goes wrong, use pre-defined std::unordered_map
    size_t last_dot = payload.find_last_of('.');
    if (last_dot== std::string::npos || last_dot == 0 || last_dot == payload.length() - 1)
    {
        throw std::runtime_error("invalid file extension");
    }
    
    std::string ext = payload.substr(last_dot + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), [](char c) {return std::tolower(c);});

    static const std::unordered_map<std::string, std::string> ext_to_mime_type = 
    {
        {"txt", "text/plain"},
        {"html", "text/html"},
        {"htm", "text/html"},
        {"jpg", "image/jpeg"},
        {"jpeg", "image/jpeg"},
        {"png", "image/png"},
        {"gif", "image/gif"},
        {"pdf", "application/pdf"},
        {"json", "application/json"}
    };

    std::string mime_type = "application/octet-stream";

    if (ext_to_mime_type.find(ext) != ext_to_mime_type.end())
    {
        mime_type = ext_to_mime_type.at(ext);
    }

    return mime_type;
}

std::string get_cmd_output(const std::string &cmd)
{
    std::array<char, 128> buffer{};
    std::string result;

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";

    while (fgets(buffer.data(), buffer.size(), pipe))
    {
        result += buffer.data();
    }

    pclose(pipe);

    result.erase(result.find_last_not_of(" \n\r\t") + 1);
    return result;

}
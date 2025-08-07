#include "X11ClipboardManager.hpp"

// C++ STD headers
#include <stdexcept>
#include <array>
#include <iostream>

// X11 headers
#include <X11/Xatom.h>

X11ClipboardManager::X11ClipboardManager(PayloadType payload_type, std::string payload)
: payload_type(payload_type), payload(payload)
{
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

void X11ClipboardManager::loop()
{
    Atom utf8 = XInternAtom(dpy, "UTF8_STRING", False);
    Atom text = XInternAtom(dpy, "TEXT", False);
    Atom targets = XInternAtom(dpy, "TARGETS", False);
    Atom string = XInternAtom(dpy, "STRING", False);

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
                if (an) XFree(an);
            
                if (requested == targets) {

                    Atom supported[] = { utf8, string, text, targets };
                    XChangeProperty(dpy, sev->requestor, sev->property, XA_ATOM, 32,
                                    PropModeReplace, (unsigned char*)supported, sizeof(supported)/sizeof(Atom));
                }
                else if (sev->property != None && (requested == utf8 || requested == string || requested == text)) {
                    send_text(sev, utf8);
                }
                else {
                    deny_request(sev);
                }
            
                // temporary workaround
                XSelectionEvent ssev;
                ssev.type = SelectionNotify;
                ssev.requestor = sev->requestor;
                ssev.selection = sev->selection;
                ssev.target = sev->target;
                ssev.property = sev->property;
                ssev.time = sev->time;
            
                XSendEvent(dpy, sev->requestor, True, NoEventMask, (XEvent *)&ssev);
                XFlush(dpy);
                break;
            }
        }
    }
}

void X11ClipboardManager::send_text(XSelectionRequestEvent *sev, Atom atom)
{
    XSelectionEvent ssev;
    time_t now_tm;

    char* an = XGetAtomName(dpy, sev->property);
    if (an)
        XFree(an);

    XChangeProperty(dpy, sev->requestor, sev->property, XInternAtom(dpy, "UTF8_STRING", False), 8, PropModeReplace,
                    (unsigned char *)(payload.c_str()), payload.size());

    ssev.type = SelectionNotify;
    ssev.requestor = sev->requestor;
    ssev.selection = sev->selection;
    ssev.target = sev->target;
    ssev.property = sev->property;
    ssev.time = sev->time;

    XSendEvent(dpy, sev->requestor, True, NoEventMask, (XEvent *)&ssev);
}

void X11ClipboardManager::deny_request(XSelectionRequestEvent* sev)
{
    XSelectionEvent ssev;
    char *an;

    an = XGetAtomName(dpy, sev->target);
    if (an)
        XFree(an);

    ssev.type = SelectionNotify;
    ssev.requestor = sev->requestor;
    ssev.selection = sev->selection;
    ssev.target = sev->target;
    ssev.property = None;
    ssev.time = sev->time;

    XSendEvent(dpy, sev->requestor, True, NoEventMask, (XEvent *)&ssev);
}
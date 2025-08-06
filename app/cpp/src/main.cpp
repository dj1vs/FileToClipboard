// STD C++ headers
#include <iostream>
#include <string>

// STD C headers
#include <string.h>

// X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>

void
send_no(Display *dpy, XSelectionRequestEvent *sev)
{
    XSelectionEvent ssev;
    char *an;

    an = XGetAtomName(dpy, sev->target);
    printf("Denying request of type '%s'\n", an);
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

void send_utf8(Display *dpy, XSelectionRequestEvent *sev, Atom utf8, std::string response_text)
{
    XSelectionEvent ssev;
    time_t now_tm;

    char* an = XGetAtomName(dpy, sev->property);
    printf("Sending data to window 0x%lx, property '%s'\n", sev->requestor, an);
    if (an)
        XFree(an);

    XChangeProperty(dpy, sev->requestor, sev->property, utf8, 8, PropModeReplace,
                    (unsigned char *)(response_text.c_str()), response_text.size());

    ssev.type = SelectionNotify;
    ssev.requestor = sev->requestor;
    ssev.selection = sev->selection;
    ssev.target = sev->target;
    ssev.property = sev->property;
    ssev.time = sev->time;

    XSendEvent(dpy, sev->requestor, True, NoEventMask, (XEvent *)&ssev);
}


int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "argc should be 3, not " + std::to_string(argc) << std::endl;
        return EXIT_FAILURE;
    }

    std::string data_format(argv[1]);
    std::string data(argv[2]);

    if (data_format == "FILE")
    {
        std::cout << "Chosen file: " << data << std::endl;
    }
    else if (data_format == "STRING")
    {
        std::cout << "Given string: " << data << std::endl;
    }
    else
    {
        std::cerr << "data format should be either FILE or STRING, not \"" + data_format + '\"' << std::endl;
        return EXIT_FAILURE;
    }

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy)
    {
        std::cerr << "Couldn't open X Display" << std::endl;
        return EXIT_FAILURE;
    }

    int screen = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);
    Window owner = XCreateSimpleWindow(dpy, root, -10, -10, 1, 1, 0, 0, 0);

    Atom sel = XInternAtom(dpy, "CLIPBOARD", False);
    Atom primary = XInternAtom(dpy, "PRIMARY", False);
    Atom utf8 = XInternAtom(dpy, "UTF8_STRING", False);
    Atom text = XInternAtom(dpy, "TEXT", False);
    Atom targets = XInternAtom(dpy, "TARGETS", False);
    Atom string = XInternAtom(dpy, "STRING", False);

    XSetSelectionOwner(dpy, sel, owner, CurrentTime);
    if (XGetSelectionOwner(dpy, sel) != owner) {
        std::cerr << "Failed to become selection owner\n";
        return EXIT_FAILURE;
    }

    XEvent x_event;
    while (true)
    {
        XNextEvent(dpy, &x_event);
        switch (x_event.type)
        {
            case SelectionClear:
                printf("Lost selection ownership\n");
                return 1;
                break;
            case SelectionRequest:
            {
                XSelectionRequestEvent* sev = (XSelectionRequestEvent*)&x_event.xselectionrequest;
                Atom targets = XInternAtom(dpy, "TARGETS", False);
            
                printf("Requestor: 0x%lx\n", sev->requestor);
                Atom requested = sev->target;
                char* an = XGetAtomName(dpy, requested);
                std::cout << "Requested target: " << (an ? an : "null") << std::endl;
                if (an) XFree(an);
            
                if (requested == targets) {
                    Atom supported[] = { utf8, string, text, targets };
                    XChangeProperty(dpy, sev->requestor, sev->property, XA_ATOM, 32,
                                    PropModeReplace, (unsigned char*)supported, sizeof(supported)/sizeof(Atom));
                }
                else if (sev->property != None && (requested == utf8 || requested == string || requested == text)) {
                    send_utf8(dpy, sev, utf8, data);
                }
                else {
                    send_no(dpy, sev);
                }
            
                // Always send response
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

            default:
                std::cout << "waiting here...\n";
                break;
        }
    }
    
}
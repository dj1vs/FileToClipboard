#ifndef _X11CLIPBOARDMANAGER_HPP_
#define _X11CLIPBOARDMANAGER_HPP_

// C++ STD headers
#include <string>
#include <vector>

// X11 headers
#include <X11/Xlib.h>

// FileToClipboard
#include "ClipboardManager.hpp"

class X11ClipboardManager : public ClipboardManager
{
public:
    X11ClipboardManager(PayloadType payload_type, std::string payload);

    void run() override;
private:
    void send_msg(XSelectionRequestEvent *sev, Atom atom);
    void deny_request(XSelectionRequestEvent* sev);

    std::string get_file_mime_type();
private:
    Display *dpy;
};

#endif // _X11CLIPBOARDMANAGER_HPP_
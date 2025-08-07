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

    void loop() override;
private:
    void send_text(XSelectionRequestEvent *sev, Atom atom);
    void deny_request(XSelectionRequestEvent* sev);
private:
    Display *dpy;
};

#endif // _X11CLIPBOARDMANAGER_HPP_
#ifndef _X11CLIPBOARDMANAGER_HPP_
#define _X11CLIPBOARDMANAGER_HPP_

// C++ STD headers
#include <string>
#include <vector>

// X11 headers
#include <X11/Xlib.h>

enum class PayloadType {String, File};

class X11ClipboardManager
{
public:
    X11ClipboardManager(PayloadType payload_type = PayloadType::String, std::string payload = "X11ClipboardManager");

    void loop();
private:
    void send_text(XSelectionRequestEvent *sev, Atom atom);
    void deny_request(XSelectionRequestEvent* sev);
private:
    PayloadType payload_type;
    std::string payload;

    Display *dpy = nullptr;
};

#endif // _X11CLIPBOARDMANAGER_HPP_
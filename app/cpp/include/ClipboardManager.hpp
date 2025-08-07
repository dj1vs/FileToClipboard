#ifndef _CLIPBOARDMANAGER_HPP_
#define _CLIPBOARDMANAGER_HPP_

// C++ STD
#include <string>

enum class PayloadType {String, File};

class ClipboardManager
{
public:
    ClipboardManager(PayloadType payload_type, std::string payload)
        : payload_type(payload_type), payload(payload) {}

    virtual void loop() {};
protected:
    PayloadType payload_type;
    std::string payload;
};

#endif // _CLIPBOARDMANAGER_HPP_
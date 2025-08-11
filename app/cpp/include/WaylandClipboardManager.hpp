#ifndef _WAYLANDCLIPBOARDMANAGER_HPP_
#define _WAYLANDCLIPBOARDMANAGER_HPP_

// C++ STD headers
#include <string>
#include <vector>

// FileToClipboard
#include "ClipboardManager.hpp"

class WaylandClipboardManager : public ClipboardManager
{
public:
    WaylandClipboardManager(PayloadType payload_type, std::string payload);

    void run() override;
};

#endif // _WAYLANDCLIPBOARDMANAGER_HPP_
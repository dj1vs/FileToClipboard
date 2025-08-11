#include "WaylandClipboardManager.hpp"

#include <stdexcept>


WaylandClipboardManager::WaylandClipboardManager(PayloadType payload_type, std::string payload)
: ClipboardManager(payload_type, payload)
{
    if (payload_type == PayloadType::File) this->payload = ClipboardManager::get_correct_file_payload_uri(payload);
}

void WaylandClipboardManager::run()
{
    FILE *pipe = popen("wl-copy", "w");
    if (!pipe) {
        throw std::runtime_error("Failed to run wl-copy");
    }
    fwrite(payload.c_str(), 1, payload.size(), pipe);
    pclose(pipe);
}
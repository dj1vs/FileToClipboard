// STD C++ headers
#include <iostream>
#include <string>
#include <unordered_map>

// STD C headers
#include <string.h>

// X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>

// FileToClipboard
#include "X11ClipboardManager.hpp"


int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "argc should be 3, not " + std::to_string(argc) << std::endl;
        return EXIT_FAILURE;
    }

    std::string data_format(argv[1]);
    std::string data(argv[2]);

    static const std::unordered_map<std::string, PayloadType> data_format_to_payload = {
        {"FILE", PayloadType::File},
        {"STRING", PayloadType::String}
    };

    if (data_format_to_payload.find(data_format) == data_format_to_payload.end())
    {
        std::cerr << "invalid data format" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        X11ClipboardManager clipboard_manager(data_format_to_payload.at(data_format), data);
        clipboard_manager.loop();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
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
#include "ClipboardManager.hpp"
#include "X11ClipboardManager.hpp"
#include <memory>


int main(int argc, char** argv)
{
    // command line arguments parsing
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

    std::shared_ptr<ClipboardManager> clipboard_manager;

    try
    {
        if (std::getenv("WAYLAND_DISPLAY"))
        {
            std::cout << "running in Wayland mode\n";
            //TODO: use WaylandClipboardManager
            clipboard_manager = std::make_shared<X11ClipboardManager>(data_format_to_payload.at(data_format), data);
        }
        else if (std::getenv("DISPLAY"))
        {
            std::cout << "running in X11 mode\n";
            clipboard_manager = std::make_shared<X11ClipboardManager>(data_format_to_payload.at(data_format), data);
        }
        else
        {
            std::cerr << "this program is unable to work on machines without X11 or Wayland" << std::endl;
            return EXIT_FAILURE;
        }
        
        clipboard_manager->loop();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
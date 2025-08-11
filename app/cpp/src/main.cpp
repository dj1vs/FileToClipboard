// STD C++ headers
#include <iostream>
#include <string>
#include <unordered_map>

// STD C/C++ headers
#include <memory>
#include <string.h>

// FileToClipboard
#include "ClipboardManager.hpp"
#include "X11ClipboardManager.hpp"
#include "WaylandClipboardManager.hpp"


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

    // guessing and launching appropriate clipboard manager from context

    std::shared_ptr<ClipboardManager> clipboard_manager;
    try
    {
        if (std::getenv("WAYLAND_DISPLAY"))
        {
            std::cout << "running in Wayland mode\n";

            try
            {
                // first try to run in x11-compatability mode
                clipboard_manager = std::make_shared<X11ClipboardManager>(data_format_to_payload.at(data_format), data);
            }
            catch (...)
            {
                // if that didn't work try to work with wayland's wl-copy
                clipboard_manager.reset();
                clipboard_manager = std::make_shared<WaylandClipboardManager>(data_format_to_payload.at(data_format), data);
            }
        }
        else if (std::getenv("DISPLAY"))
        {
            std::cout << "running in X11 mode\n";
            clipboard_manager = std::make_shared<X11ClipboardManager>(data_format_to_payload.at(data_format), data);
        }
        else
        {
            std::cerr << "cannot guess context in which i'm running" << std::endl;
            return EXIT_FAILURE;
        }

        clipboard_manager->run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
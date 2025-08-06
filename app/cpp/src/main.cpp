#include <iostream>
#include <string>

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

    
}
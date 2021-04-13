#include <filesystem>
#include <iostream>

int main(int argc, char* argv[])
{
    using namespace std::filesystem;

    if (argc != 4) {
        std::cerr << "Wrong number of arguments" << std::endl;
        return 1;
    }

    path inputPath(argv[1]);
    path outputPath = path(argv[2]) / path(argv[3]);

    if (!is_regular_file(inputPath)) {
        std::cerr << "Input path incorrect" << std::endl;
        return 2;
    }

    if (!copy_file(inputPath, outputPath, copy_options::overwrite_existing)) {
        std::cerr << "Copy failed" << std::endl;
        return 3;
    }

    return 0;
}

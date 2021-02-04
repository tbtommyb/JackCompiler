#include "src/VM.hpp"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "USAGE: vm input.vm" << std::endl;
        exit(1);
    }

    fs::path input{argv[1]};
    std::ofstream outputFile{input.stem().string() + ".asm"};
    CodeWriter writer{outputFile};
    VM vm{writer};

    if (fs::is_directory(input)) {
        for (const auto &entry : fs::directory_iterator(input)) {

            if (entry.path().extension() == ".vm") {
                std::ifstream inputFile{entry.path().string()};
                vm.process(inputFile, entry.path().stem().string());
            }
        }
    } else {
        std::ifstream inputFile{input.string()};
        vm.process(inputFile, input.stem().string());
    }

    return 0;
};

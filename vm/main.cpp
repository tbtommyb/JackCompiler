#include "src/VM.hpp"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "USAGE: vm input.vm" << std::endl;
        exit(1);
    }

    fs::path input{argv[1]};
    std::ofstream output{input.stem().string() + ".asm"};
    VM vm{output};
    vm.run(input);

    return 0;
};

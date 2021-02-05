#include "src/Compiler.hpp"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "USAGE: JackAnalyser [file.jack|dir]" << std::endl;
        exit(1);
    }

    Compiler compiler{};
    compiler.run(argv[1]);

    return 0;
};

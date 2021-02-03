#include "src/Assemblr.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "USAGE: assemblr input.asm" << std::endl;
        exit(1);
    }
    auto input = argv[1];
    std::ifstream program{input};

    auto filename = getFilename(input);
    std::ofstream out{filename + ".hack"};

    Assemble(program, out);
    out.close();

    return 0;
};

#include "src/Assemblr.hpp"

std::string getFilename(std::string input) {
    std::istringstream iss{input};
    if (std::getline(iss, input, '.')) {
        return input;
    }
    return "";
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "USAGE: assemblr input.asm" << std::endl;
        exit(1);
    }
    auto *input = argv[1];
    std::ifstream program{input};

    auto filename = getFilename(input);
    std::ofstream out{filename + ".hack"};

    Assemblr{program, out}.run();
    out.close();

    return 0;
};

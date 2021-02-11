#include "Compiler.hpp"

namespace fs = boost::filesystem;

void Compiler::run(std::string dir) {
    fs::path input{dir};
    std::vector<fs::path> filesToProcess{};

    if (fs::is_directory(input)) {
        for (const auto &entry : fs::directory_iterator(input)) {
            if (entry.path().extension() == ".jack") {
                filesToProcess.push_back(entry.path());
            }
        }
    } else {
        filesToProcess.push_back(input);
    }

    for (const auto &filePath : filesToProcess) {
        std::ifstream file{filePath.string()};
        std::ofstream outputFile{filePath.stem().string() + ".vm"};

        assert(file.is_open());
        assert(outputFile.is_open());
        process(file, outputFile);
    }
};

void Compiler::process(std::istream &program, std::ostream &out) {
    JackTokenizer tokenizer{program};
    TokenList tokens = tokenizer.tokenize();

    CompilationEngine compiler{tokens, out};
    compiler.compile();
};

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
        assert(file.is_open());

        std::string outputPath = filePath.stem().string() + ".vm";
        std::ostringstream output{};

        if (process(file, output)) {
            std::ofstream outputFile{outputPath};
            assert(outputFile.is_open());
            outputFile << output.str() << std::endl;
        }
    }
};

bool Compiler::process(std::istream &program, std::ostream &out) {
    JackTokenizer tokenizer{program};
    TokenList tokens = tokenizer.tokenize();

    CompilationEngine compiler{tokens, out};
    return compiler.compile();
};

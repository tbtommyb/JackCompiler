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
        std::string inputPath{filePath.string()};
        assert(file.is_open());

        std::string outputPath = filePath.stem().string() + ".vm";
        std::ostringstream output{};

        if (process(inputPath, file, output)) {
            std::ofstream outputFile{outputPath};
            assert(outputFile.is_open());
            outputFile << output.str() << std::endl;
        }
    }
};

bool Compiler::process(const std::string &inputPath, std::istream &program,
                       std::ostream &out) {
    try {
        JackTokenizer tokenizer{program};
        TokenList tokens = tokenizer.tokenize();

        CompilationEngine compiler{tokens, out};
        compiler.compile();
        return true;
    } catch (const CompilationError &e) {
        std::cerr << "Compilation error in " << inputPath << ":\n"
                  << e.what() << std::endl;
        return false;
    } catch (const SymbolNotFoundError &e) {
        std::cerr << "Compilation error in " << inputPath << ":\n"
                  << e.what() << std::endl;
        return false;
    }
};

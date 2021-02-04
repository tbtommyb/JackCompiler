#include "VM.hpp"

VM::VM(std::ostream &output) : writer{output} {}

void VM::run(fs::path &input) {
    if (fs::is_directory(input)) {
        for (const auto &entry : fs::directory_iterator(input)) {

            if (entry.path().extension() == ".vm") {
                std::ifstream inputFile{entry.path().string()};
                process(inputFile, entry.path().stem().string());
            }
        }
    } else {
        std::ifstream inputFile{input.string()};
        process(inputFile, input.stem().string());
    }
}

void VM::process(std::istream &input, const std::string inputName) {
    writer.setCurrentFile(inputName);

    Parser parser{input};

    while (parser.hasMoreCommands()) {
        parser.advance();

        Command command = parser.parse();
        switch (command.type) {
        case CommandType::C_PUSH:
        case CommandType::C_POP:
            writer.writePushPop(command);
            break;
        case CommandType::C_ARITHMETIC:
            writer.writeArithmetic(command);
            break;
        case CommandType::C_LABEL:
            writer.writeLabel(command);
            break;
        case CommandType::C_GOTO:
            writer.writeGoto(command);
            break;
        case CommandType::C_IF:
            writer.writeIf(command);
            break;
        case CommandType::C_CALL:
            writer.writeCall(command);
            break;
        case CommandType::C_FUNCTION:
            writer.writeFunction(command);
            break;
        case CommandType::C_RETURN:
            writer.writeReturn();
            break;
        }
    }
};

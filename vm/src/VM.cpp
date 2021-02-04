#include "VM.hpp"

VM::VM(CodeWriter &writer) : writer(writer) {}

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

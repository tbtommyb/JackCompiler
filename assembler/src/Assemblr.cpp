#include "Assemblr.hpp"

void Assemblr::buildSymbolTable(SymbolTable &table, Parser &parser) {
    unsigned int instructionAddress = 0x0000;

    while (parser.hasMoreCommands()) {
        parser.advance();
        Instruction instruction;

        try {
            instruction = parser.parse();
        } catch (const InvalidCommand &e) {
            std::cerr << "Invalid command: " << e.what() << std::endl;
            exit(1);
        }

        switch (instruction.type) {
        case C_COMMAND:
        case A_COMMAND:
            instructionAddress++;
            break;
        case L_COMMAND:
            table.addEntry(instruction.symbol, instructionAddress);
            break;
        }
    }
}

void Assemblr::run() {
    buildSymbolTable(symbols, parser);
    program.seekg(0);

    while (parser.hasMoreCommands()) {
        parser.advance();
        Instruction instruction;

        try {
            instruction = parser.parse();
        } catch (const InvalidCommand &e) {
            std::cerr << "Invalid command: " << e.what() << std::endl;
            exit(1);
        }

        auto code = Code{instruction, symbols};
        if (instruction.type != L_COMMAND) {
            out << code.string() << std::endl;
        }
    }
}

Assemblr::Assemblr(std::istream &program, std::ostream &out)
    : out(out), program(program), symbols(SymbolTable{}),
      parser(Parser{program}) {}

#include "Parser.hpp"

const std::regex Parser::A_command{"^@([0-9]+|[a-zA-Z_\\.\\$0-9]+)"};
const std::regex Parser::L_command{"\\(([a-zA-Z_\\.\\$0-9]+)\\)"};
const std::regex Parser::C_command{"([A-Z]{1,3})=(.+);([A-Z]{3})"};
const std::regex Parser::C_command_no_dest{"(.+);([A-Z]{3})"};
const std::regex Parser::C_command_no_jump{"([A-Z]{1,3})=(.+)"};

Parser::Parser(std::istream &input) : stream(input){};

const Instruction Parser::parse() {
    auto type = commandType();
    Instruction instruction{};
    instruction.type = type;

    if (type == C_COMMAND) {
        instruction.dest = dest();
        instruction.comp = comp();
        instruction.jump = jump();
    } else {
        instruction.symbol = symbol();
    }

    return instruction;
}

bool Parser::hasMoreCommands() {
    auto c = stream.peek();
    if (c == EOF) {
        return false;
    }
    return true;
};

void Parser::advance() {
    std::string input;
    std::getline(stream, input);
    currentLine = sanitise(input);

    // Skip empty lines
    if (currentLine.length() == 0) {
        advance();
    }
};

CommandType Parser::commandType() {
    std::smatch match;

    if (std::regex_match(currentLine, match, A_command)) {
        if (match.size() == 2) {
            A_value = match[1].str();

            return CommandType::A_COMMAND;
        }
    }

    if (std::regex_match(currentLine, match, L_command)) {
        if (match.size() == 2) {
            A_value = match[1].str();

            return CommandType::L_COMMAND;
        }
    }

    if (std::regex_match(currentLine, match, C_command)) {
        if (match.size() == 4) {
            C_dest = match[1].str();
            C_comp = match[2].str();
            C_jump = match[3].str();

            return CommandType::C_COMMAND;
        }
    }

    if (std::regex_match(currentLine, match, C_command_no_dest)) {
        if (match.size() == 3) {
            C_comp = match[1].str();
            C_jump = match[2].str();

            C_dest = "";

            return CommandType::C_COMMAND;
        }
    }

    if (std::regex_match(currentLine, match, C_command_no_jump)) {
        if (match.size() == 3) {
            C_dest = match[1].str();
            C_comp = match[2].str();

            C_jump = "";

            return CommandType::C_COMMAND;
        }
    }

    throw InvalidCommand{currentLine};
};

const std::string &Parser::symbol() const { return A_value; };

const std::string &Parser::dest() const { return C_dest; };

const std::string &Parser::comp() const { return C_comp; };

const std::string &Parser::jump() const { return C_jump; };

std::string Parser::sanitise(std::string s) {
    s.erase(std::remove_if(s.begin(), s.end(),
                           [](unsigned char x) { return std::isspace(x); }),
            s.end());
    auto commentPos = s.find("//");
    if (commentPos != std::string::npos) {
        s.erase(commentPos);
    }
    return s;
};

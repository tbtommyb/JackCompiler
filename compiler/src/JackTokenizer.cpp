#include "JackTokenizer.hpp"

JackTokenizer::JackTokenizer(std::istream &in)
    : it{in}, end{}, lineNumber{1} {};

TokenList JackTokenizer::tokenize() {
    while (moreCharsRemain()) {
        std::string lexeme = scan();
        if (!lexeme.empty()) {
            tokens.push_back(toToken(lexeme));
        }
    }

    tokens.push_back(Token{TokenType::COMP_EOF, "", lineNumber});

    return tokens;
};

bool JackTokenizer::moreCharsRemain() {
    while (isspace(peek())) {
        advance();
    }

    skipCommentBlock();

    return it != end;
};

std::string JackTokenizer::scan() {
    std::string lexeme;
    char c;

    do {
        c = advance();
        lexeme += c;
    } while (isalnum(c));

    if (lexeme[0] == '\n') {
        lineNumber++;
        return "";
    }
    if (lexeme[0] == '\"') {
        while (peek() != '\"') {
            if (peek() == '\n') {
                lineNumber++;
            }
            lexeme += advance();
        }
        lexeme += advance(); // add the second "
    }

    return lexeme;
}

void JackTokenizer::skipCommentBlock() {
    if (peek() == '/' && peekNext() == '/') {
        std::advance(it, 2);
        while (peek() != '\n') {
            advance();
        }
    }

    if (!(peek() == '/' && peekNext() == '*')) {
        return;
    }

    std::advance(it, 2);

    while (!(peek() == '*' && peekNext() == '/')) {
        advance();
    }

    std::advance(it, 2);
};

char JackTokenizer::advance() {
    return *it++; // increment, return prev
};

char JackTokenizer::peek() { return *it; }
char JackTokenizer::peekNext() { return *(std::next(it)); }

Token JackTokenizer::toToken(const std::string &input) {
    TokenType tokenType;

    auto token = tokenMap.find(input);
    if (token != tokenMap.end()) {
        tokenType = token->second;
    } else if (isInteger(input)) {
        tokenType = TokenType::INTEGER;
    } else if (isString(input)) {
        // Strip double quotes
        return Token{TokenType::STRING, input.substr(1, input.size() - 2),
                     lineNumber};
    } else if (isIdentifier(input)) {
        tokenType = TokenType::IDENTIFIER;
    } else {
        throw new CompilationError{"Cannot tokenize " + input};
    }

    return Token{tokenType, input, lineNumber};
};

bool JackTokenizer::isInteger(const std::string &input) {
    std::string::const_iterator it = input.begin();

    while (it != input.end() && isdigit(*it)) {
        ++it;
    }

    if (it == input.end() && input.length() > 0) {
        int num = std::stoi(input);
        return num >= 0 && num <= 32767; // TODO: why this?
    }

    return false;
};

bool JackTokenizer::isString(const std::string &input) {
    const std::regex pattern{R"(^\"[[:print:]]+\"$)"};
    return std::regex_match(input, pattern);
};

bool JackTokenizer::isIdentifier(const std::string &input) {
    const std::regex pattern{R"(^[a-zA-Z_]+[a-zA-Z0-9_]*)"};
    return std::regex_match(input, pattern);
};

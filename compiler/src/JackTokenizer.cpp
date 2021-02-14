#include "JackTokenizer.hpp"

JackTokenizer::JackTokenizer(std::istream &in) : it{in}, lineNumber{1} {};

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

    return !it.eof();
};

std::string JackTokenizer::scan() {
    std::string lexeme;
    char c = advance();

    lexeme += c;
    while (isalnum(c) && isalnum(peek())) {
        c = advance();
        lexeme += c;
    }

    if (lexeme[0] == '\n') {
        return "";
    }
    if (lexeme[0] == '\"') {
        while (peek() != '\"') {
            lexeme += advance();
        }
        lexeme += advance(); // add the second "
    }

    return lexeme;
}

void JackTokenizer::skipCommentBlock() {
    if (peek() == '/' && peekNext() == '/') {
        it.seekg(2, std::ios::cur);
        while (peek() != '\n') {
            advance();
        }
        // lineNumber++;
    }

    if (!(peek() == '/' && peekNext() == '*')) {
        return;
    }

    it.seekg(2, std::ios::cur);

    while (!(peek() == '*' && peekNext() == '/')) {
        advance();
    }

    it.seekg(2, std::ios::cur);
};

char JackTokenizer::advance() {
    char c = it.get();
    if (c == '\n') {
        lineNumber++;
    }
    return c;
}

char JackTokenizer::peek() { return it.peek(); }

char JackTokenizer::peekNext() {
    it.get();
    char c = it.peek();
    it.unget();
    return c;
}

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
        throw CompilationError{"Cannot tokenize " + input};
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

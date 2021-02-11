#ifndef __JackTokenizer__
#define __JackTokenizer__

#include "CompilationError.hpp"
#include "Token.hpp"
#include "TokenType.hpp"
#include <iostream>
#include <istream>
#include <iterator>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using TokenList = std::vector<Token>;

// clang-format off
const std::map<std::string, TokenType> tokenMap = {
    {"boolean", TokenType::BOOLEAN},
    {"char", TokenType::CHAR},
    {"class", TokenType::CLASS},
    {"constructor", TokenType::CONSTRUCTOR},
    {"do", TokenType::DO},
    {"else", TokenType::ELSE},
    {"false", TokenType::COMP_FALSE},
    {"field", TokenType::FIELD},
    {"function", TokenType::FUNCTION},
    {"if", TokenType::IF},
    {"int", TokenType::INT},
    {"let", TokenType::LET},
    {"method", TokenType::NOT},
    {"null", TokenType::COMP_NULL},
    {"return", TokenType::RETURN},
    {"static", TokenType::STATIC},
    {"this", TokenType::THIS},
    {"true", TokenType::COMP_TRUE},
    {"var", TokenType::VAR},
    {"void", TokenType::VOID},
    {"while", TokenType::WHILE},
    {",", TokenType::COMMA},
    {"=", TokenType::EQUALS},
    {".", TokenType::FULL_STOP},
    {"-", TokenType::HYPHEN},
    {"{", TokenType::LBRACE},
    {"}", TokenType::RBRACE},
    {"(", TokenType::LPAREN},
    {")", TokenType::RPAREN},
    {"[", TokenType::LBRACKET},
    {"]", TokenType::RBRACKET},
    {";", TokenType::SEMICOLON},
    {"~", TokenType::TILDE},
    {"+", TokenType::PLUS},
    {"*", TokenType::STAR},
    {"/", TokenType::FORWARD_SLASH},
    {"&", TokenType::AMPERSAND},
    {"|", TokenType::BAR},
    {"<", TokenType::LESS_THAN},
    {">", TokenType::GREATER_THAN}
};
// clang-format on

class JackTokenizer {
  public:
    explicit JackTokenizer(std::istream &);
    TokenList tokenize();

  private:
    std::string scan();
    Token toToken(const std::string &);
    bool moreCharsRemain();
    void skipCommentBlock();
    char advance();
    char peek();
    char peekNext();
    bool isInteger(const std::string &);
    bool isString(const std::string &);
    bool isIdentifier(const std::string &);
    std::istream_iterator<char> it;
    std::istream_iterator<char> end;
    int lineNumber;
    TokenList tokens;
};

#endif

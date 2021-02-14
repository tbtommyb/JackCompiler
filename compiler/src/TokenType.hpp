#ifndef __TokenType__
#define __TokenType__

#include <map>
#include <string>

// clang-format on
enum class TokenType {
    BOOLEAN,
    CHAR,
    CLASS,
    CONSTRUCTOR,
    DO,
    ELSE,
    COMP_FALSE,
    FIELD,
    FUNCTION,
    IF,
    INT,
    LET,
    METHOD,
    NOT,
    COMP_NULL,
    RETURN,
    STATIC,
    THIS,
    COMP_TRUE,
    VAR,
    WHILE,
    VOID,

    // types
    INTEGER,
    STRING,
    IDENTIFIER,

    // symbols
    AMPERSAND,
    BAR,
    COMMA,
    EQUALS,
    FORWARD_SLASH,
    FULL_STOP,
    GREATER_THAN,
    HYPHEN,
    LBRACE,
    LBRACKET,
    LESS_THAN,
    LPAREN,
    PLUS,
    RBRACE,
    RBRACKET,
    RPAREN,
    SEMICOLON,
    STAR,
    TILDE,

    COMP_EOF
};
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

inline std::string tokenString(const TokenType &tt) {
    for (const auto &[key, value] : tokenMap) {
       if (tt == value) {
            return key;
       }
    }
    return "Unknown token type";
};


#endif

#ifndef __TokenType__
#define __TokenType__

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
#endif

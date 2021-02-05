#ifndef __JackTokenizer__
#define __JackTokenizer__

#include "Tokens.hpp"
#include <iostream>
#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

enum class Keyword {
    CLASS,
    CONSTRUCTOR,
    FUNCTION,
    METHOD,
    FIELD,
    STATIC,
    VAR,
    INT,
    CHAR,
    BOOLEAN,
    VOID,
    COMP_TRUE,
    COMP_FALSE,
    NULL_VAL,
    THIS,
    LET,
    DO,
    IF,
    ELSE,
    WHILE,
    RETURN
};

using TokenList = std::vector<std::shared_ptr<Token>>;

class JackTokenizer {
  public:
    explicit JackTokenizer(std::istream &);
    std::vector<std::shared_ptr<Token>> getTokenList();

  private:
    bool isRemainingChar(std::string::iterator &);
    bool isCommentLine(std::string::iterator &);
    void skipCommentBlock(std::string::iterator &);
    std::shared_ptr<Token> nextToken(const std::string &);
    bool isKeyword(const std::string &);
    bool isSymbol(char16_t);
    bool isInteger(const std::string &);
    bool isString(const std::string &);
    bool isIdentifier(const std::string &);
    std::istream &input;
    std::string currentLine;
    int lineNumber;
    bool multilineCommentBlock;
};

#endif

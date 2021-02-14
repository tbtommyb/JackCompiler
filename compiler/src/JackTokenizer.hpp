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
    std::istream& it;
    int lineNumber;
    TokenList tokens;
};

#endif

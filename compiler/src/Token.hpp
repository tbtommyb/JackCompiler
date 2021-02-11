#ifndef __Token__
#define __Token__

#include "TokenType.hpp"
#include <string>

struct Token {
    TokenType tokenType;
    std::string src;
    int lineNumber;
};

#endif

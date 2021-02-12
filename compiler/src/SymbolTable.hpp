#ifndef __SymbolTable__
#define __SymbolTable__

#include <map>
#include <sstream>
#include <string>

#include "CompilationError.hpp"
#include "Token.hpp"

class SymbolNotFoundError : public std::exception {
  public:
    SymbolNotFoundError(std::string msg) : msg(msg.c_str()) {}
    [[nodiscard]] const char *what() const noexcept override { return msg; }

  private:
    const char *msg;
};

struct SymbolKind {
    enum Enum { NONE, STATIC, FIELD, ARGUMENT, VAR };
    static std::string toString(const SymbolKind::Enum &kind) {
        switch (kind) {
        case NONE:
            return "none";
        case STATIC:
            return "static";
        case FIELD:
            return "field";
        case ARGUMENT:
            return "argument";
        case VAR:
            return "var";
        }
    }
};

struct Symbol {
    std::string name;
    std::string type;
    SymbolKind::Enum kind;
    int id;
    [[nodiscard]] std::string toString() const;
};

class SymbolTable {
  public:
    SymbolTable() = default;
    ~SymbolTable() = default;
    void startSubroutine();
    Symbol addSymbol(Token name, Token type, Token kind);
    Symbol addSymbol(Token name, Token type, const SymbolKind::Enum &kind);
    Symbol addSymbol(const std::string &name, const std::string &type,
                     const SymbolKind::Enum &kind);
    Symbol getSymbol(Token name);
    int getCount(const SymbolKind::Enum &kind);

  private:
    std::map<std::string, Symbol> classMap;
    std::map<std::string, Symbol> subroutineMap;
    int staticCount = 0;
    int fieldCount = 0;
    int argumentCount = 0;
    int varCount = 0;
};

#endif

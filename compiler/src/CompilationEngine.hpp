#ifndef __CompilationEngine__
#define __CompilationEngine__

#include "CompilationError.hpp"
#include "JackTokenizer.hpp"
#include "SymbolTable.hpp"
#include "Token.hpp"
#include "TokenType.hpp"
#include "VMWriter.hpp"
#include <iostream>
#include <string>

using MatchOptions = const std::vector<TokenType>&;

class CompilationEngine {
  public:
    CompilationEngine(TokenList &tokens, std::ostream &);
    ~CompilationEngine() = default;
    bool compile();
    bool compileClass();
    bool compileClassVarDec();
    bool compileSubroutineDec();
    bool compileParameterList();
    bool compileVarDec();
    bool compileSubroutineBody(Token name, Token keyword);
    bool compileStatements();
    bool compileStatement();
    bool compileLet();
    bool compileIf();
    bool compileWhile();
    bool compileDo();
    bool compileReturn();
    bool compileExpression();
    bool compileTerm();
    bool compileSubroutineCall();
    int compileExpressionList();
    bool compileUnaryOp();
    bool compileKeywordConstant();
    bool compileIntConst();
    bool compileStringConst();

  private:
    Token consume(MatchOptions);
    bool match(MatchOptions);
    bool matchNext(MatchOptions);
    Token readType();
    bool zeroOrOnce(const std::function<void(void)> &);
    bool zeroOrMany(const std::function<bool(void)> &);
    const std::string expected(MatchOptions, const Token &);
    const std::string newLabel();
    TokenList::iterator token;
    VMWriter vmWriter;
    std::string className;
    SymbolTable symbolTable;
    int labelCount;
};

#endif

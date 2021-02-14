#ifndef __CompilationEngine__
#define __CompilationEngine__

#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <iostream>
#include <numeric>
#include <string>
#include "CompilationError.hpp"
#include "JackTokenizer.hpp"
#include "SymbolTable.hpp"
#include "Token.hpp"
#include "TokenType.hpp"
#include "VMWriter.hpp"

using MatchOptions = const std::vector<TokenType>&;

class CompilationEngine {
  public:
    CompilationEngine(TokenList &tokens, std::ostream &);
    ~CompilationEngine() = default;
    void compile();
    void compileClass();
    void compileClassVarDec();
    void compileSubroutineDec();
    void compileParameterList();
    void compileVarDec();
    void compileSubroutineBody(Token name, Token keyword);
    void compileStatements();
    void compileStatement();
    void compileLet();
    void compileIf();
    void compileWhile();
    void compileDo();
    void compileReturn();
    void compileExpression();
    void compileTerm();
    void compileIdentifier();
    void compileSubroutineCall();
    int compileExpressionList();
    void compileUnaryOp();
    void compileKeywordConstant();
    void compileIntConst();
    void compileStringConst();
    void compileArrayLiteral(Segment::Enum, Symbol);

  private:
    Token consume(MatchOptions);
    bool match(MatchOptions);
    bool matchNext(MatchOptions);
    Token readType();
    bool zeroOrOnce(const std::function<void(void)> &);
    bool zeroOrMany(const std::function<bool(void)> &);
    const std::string expected(MatchOptions);
    const std::string newLabel();
    TokenList::iterator token;
    VMWriter vmWriter;
    std::string className;
    SymbolTable symbolTable;
    int labelCount;
};

#endif

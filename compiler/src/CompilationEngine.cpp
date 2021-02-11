#include "CompilationEngine.hpp"

template <typename T>
bool oneOf(T t) {
    try {
        return t();
    } catch (const CompilationError &e) {
        return false;
    }
};

template <typename T, typename... As>
bool oneOf(T t, As... Fs) {
    try {
        if (t()) {
            return true;
        }
        return oneOf(Fs...);
    } catch (const CompilationError &e) {
        return oneOf(Fs...);
    }
};

template <typename T>
Token oneOfToken(T t) {
    return t();
};

template <typename T, typename... As>
Token oneOfToken(T t, As... Fs) {
    try {
        return t();
    } catch (const CompilationError &e) {
        return oneOfToken(Fs...);
    }
};

std::map<SymbolKind::Enum, Segment::Enum> kindSegmentMap = {
    {SymbolKind::STATIC, Segment::STATIC},
    {SymbolKind::FIELD, Segment::THIS},
    {SymbolKind::ARGUMENT, Segment::ARG},
    {SymbolKind::VAR, Segment::LOCAL},
};

std::map<TokenType, std::string> opCommandMap = {
    {TokenType::PLUS, "add"},
    {TokenType::HYPHEN, "sub"},
    {TokenType::STAR, "call Math.multiply 2"},
    {TokenType::FORWARD_SLASH, "call Math.divide 2"},
    {TokenType::LESS_THAN, "lt"},
    {TokenType::GREATER_THAN, "gt"},
    {TokenType::EQUALS, "eq"},
    {TokenType::AMPERSAND, "and"},
    {TokenType::BAR, "or"},
};

std::map<TokenType, std::string> unaryOpCommandMap = {
    {TokenType::HYPHEN, "neg"}, {TokenType::TILDE, "not"}};

MatchOptions ops = {
    TokenType::PLUS,          TokenType::HYPHEN,       TokenType::STAR,
    TokenType::FORWARD_SLASH, TokenType::AMPERSAND,    TokenType::BAR,
    TokenType::LESS_THAN,     TokenType::GREATER_THAN, TokenType::EQUALS};

CompilationEngine::CompilationEngine(TokenList &tokens, std::ostream &out)
    : token{tokens.begin()}, vmWriter{out}, labelCount{0}, symbolTable{} {}

// Public compilation methods
// ==========================

bool CompilationEngine::compile() {
    try {
        compileClass();
    } catch (const CompilationError &e) {
        std::cerr << "Compilation error: " << e.what() << std::endl;
        return false;
    }

    return true;
};

bool CompilationEngine::compileClass() {
    // 'class' className '{' classVarDec* subroutineDec* '}'

    consume({TokenType::CLASS});
    Token ident = consume({TokenType::IDENTIFIER});

    className = ident.src;

    consume({TokenType::LBRACE});

    zeroOrMany([this] { return compileClassVarDec(); });
    zeroOrMany([this] { return compileSubroutineDec(); });

    return true;
};

bool CompilationEngine::compileClassVarDec() {
    // ('static' | 'field' ) type varName (',' varName)* ';'

    if (!match({TokenType::STATIC, TokenType::FIELD})) {
        return false;
    }

    Token keyword = consume({TokenType::STATIC, TokenType::FIELD});
    Token type = readType();
    Token varName = consume({TokenType::IDENTIFIER});

    auto symbol = symbolTable.addSymbol(varName, type, keyword);

    while (match({TokenType::COMMA})) {
        consume({TokenType::COMMA});
        Token ident = consume({TokenType::IDENTIFIER});
        auto symbol = symbolTable.addSymbol(ident, type, keyword);
    }

    consume({TokenType::SEMICOLON});

    return true;
};

bool CompilationEngine::compileSubroutineDec() {
    // ('constructor' | 'function' | 'method')
    // ('void' | type) subroutineName '(' parameterList ')'
    // subroutineBody

    if (!match(
            {TokenType::CONSTRUCTOR, TokenType::FUNCTION, TokenType::METHOD})) {
        return false;
    }

    symbolTable.startSubroutine();
    vmWriter.write("// Compiling subroutine");

    Token keyword = consume(
        {TokenType::CONSTRUCTOR, TokenType::FUNCTION, TokenType::METHOD});
    if (keyword.tokenType == TokenType::METHOD) {
        symbolTable.addSymbol("this", className, SymbolKind::ARGUMENT);
    }

    Token type = oneOfToken([this] { return consume({TokenType::VOID}); },
                            [this] { return readType(); });
    Token ident = consume({TokenType::IDENTIFIER});

    consume({TokenType::LPAREN});
    compileParameterList();
    consume({TokenType::RPAREN});

    // TODO add type
    compileSubroutineBody(ident, keyword);
    vmWriter.write("// End subroutine");

    return true;
};

bool CompilationEngine::compileParameterList() {
    // ((type varName) (',' type varName)*)?

    // TODO: this looks wrong
    if (!(match({TokenType::INT, TokenType::CHAR, TokenType::BOOLEAN,
                 TokenType::IDENTIFIER}))) {
        return false;
    }

    if (!match({TokenType::RPAREN})) {
        Token type = readType();
        Token ident = consume({TokenType::IDENTIFIER});

        symbolTable.addSymbol(ident, type, SymbolKind::ARGUMENT);

        while (match({TokenType::COMMA})) {
            consume({TokenType::COMMA});
            Token type = readType();
            Token ident = consume({TokenType::IDENTIFIER});

            symbolTable.addSymbol(ident, type, SymbolKind::ARGUMENT);
        }
    }

    return true;
};

bool CompilationEngine::compileVarDec() {
    // 'var' type varName (',' varName)* ';'

    if (!match({TokenType::VAR})) {
        return false;
    }

    Token keyword = consume({TokenType::VAR});
    Token type = readType();
    Token ident = consume({TokenType::IDENTIFIER});

    symbolTable.addSymbol(ident, type, keyword);

    while (match({TokenType::COMMA})) {
        consume({TokenType::COMMA});
        Token ident = consume({TokenType::IDENTIFIER});
        symbolTable.addSymbol(ident, type, keyword);
    }

    consume({TokenType::SEMICOLON});

    return true;
};

bool CompilationEngine::compileSubroutineBody(Token name, Token keyword) {
    // '{' varDec* statements '}'

    if (!(match({TokenType::LBRACE}))) {
        return false;
    }

    vmWriter.write("// Compiling subroutine body");
    consume({TokenType::LBRACE});

    zeroOrMany([this] { return compileVarDec(); });

    // TODO add 1 for methods
    vmWriter.writeFunction(className + "." + name.src,
                           symbolTable.getCount(SymbolKind::VAR));

    if (keyword.tokenType == TokenType::CONSTRUCTOR) {
        vmWriter.writePush(Segment::CONST,
                           symbolTable.getCount(SymbolKind::FIELD));
        vmWriter.writeCall("Memory.alloc", 1);
        vmWriter.writePop(Segment::POINTER, 0);
    } else if (keyword.tokenType == TokenType::METHOD) {
        vmWriter.writePush(Segment::ARG, 0);
        vmWriter.writePop(Segment::POINTER, 0);
    }

    compileStatements();

    consume({TokenType::RBRACE});
    vmWriter.write("// End subroutine body");

    return true;
};

bool CompilationEngine::compileStatements() {
    // statement*

    // TODO logging semicolon so not incrementing token somewhere
    if (!match({TokenType::LET, TokenType::IF, TokenType::ELSE,
                TokenType::WHILE, TokenType::DO, TokenType::RETURN})) {
        return false;
    }

    zeroOrMany([this] { return compileStatement(); });

    return true;
};

bool CompilationEngine::compileStatement() {
    // letStatement | ifStatement | whileStatement | doStatement |
    // returnStatement

    return oneOf(
        [this] { return compileLet(); }, [this] { return compileIf(); },
        [this] { return compileWhile(); }, [this] { return compileDo(); },
        [this] { return compileReturn(); });
};

bool CompilationEngine::compileLet() {
    // 'let' varName ('[' expression ']')? '=' expression ';'

    if (!match({TokenType::LET})) {
        return false;
    }

    bool arrayAccess = false;

    vmWriter.write("// Compiling let");
    consume({TokenType::LET});

    Token ident = consume({TokenType::IDENTIFIER});
    Symbol symbol = symbolTable.getSymbol(ident);
    auto segment = kindSegmentMap.at(symbol.kind);

    if (match({TokenType::LBRACKET})) {
        consume({TokenType::LBRACKET});
        arrayAccess = true;
        vmWriter.writePush(segment, symbol.id);
        compileExpression();

        consume({TokenType::RBRACKET});
        vmWriter.write("add");
    }

    consume({TokenType::EQUALS});
    compileExpression();
    consume({TokenType::SEMICOLON});

    if (arrayAccess) {
        vmWriter.writePop(Segment::TEMP, 1);
        vmWriter.writePop(Segment::POINTER, 1);
        vmWriter.writePush(Segment::TEMP, 1);
        vmWriter.writePop(Segment::THAT, 0);
    } else {
        vmWriter.writePop(segment, symbol.id);
    }

    return true;
};

bool CompilationEngine::compileIf() {
    // 'if '(' expression ')' '{' statements '}' ('else' '{' statements
    // '}')?

    if (!match({TokenType::IF})) {
        return false;
    }

    vmWriter.write("// Compiling if");
    consume({TokenType::IF});
    auto endLabel = newLabel();

    consume({TokenType::LPAREN});
    compileExpression();
    consume({TokenType::RPAREN});

    vmWriter.write("not");
    auto notLabel = newLabel();
    vmWriter.writeIf(notLabel);

    consume({TokenType::LBRACE});
    compileStatements();
    consume({TokenType::RBRACE});

    vmWriter.writeGoto(endLabel);
    vmWriter.writeLabel(notLabel);

    if (match({TokenType::ELSE})) {
        consume({TokenType::ELSE});
        vmWriter.write("// Compiling else");
        consume({TokenType::LBRACE});
        compileStatements();
        consume({TokenType::RBRACE});
    }

    vmWriter.writeLabel(endLabel);

    return true;
};

bool CompilationEngine::compileWhile() {
    // 'while' '(' expression ')' '{' statements '}'

    if (!match({TokenType::WHILE})) {
        return false;
    }

    vmWriter.write("// Compiling while");
    consume({TokenType::WHILE});
    auto topLabel = newLabel();
    vmWriter.writeLabel(topLabel);

    consume({TokenType::LPAREN});
    compileExpression();
    consume({TokenType::RPAREN});

    vmWriter.write("not");
    auto notLabel = newLabel();
    vmWriter.writeIf(notLabel);

    consume({TokenType::LBRACE});
    compileStatements();
    consume({TokenType::RBRACE});

    vmWriter.writeGoto(topLabel);
    vmWriter.writeLabel(notLabel);

    return true;
};

bool CompilationEngine::compileDo() {
    // 'do' subroutineCall ';'

    if (!match({TokenType::DO})) {
        return false;
    }

    vmWriter.write("// Compiling do");
    consume({TokenType::DO});

    compileSubroutineCall();

    consume({TokenType::SEMICOLON});
    vmWriter.writePop(Segment::TEMP, 0);

    return true;
};

bool CompilationEngine::compileReturn() {
    // 'return' expression? ';'

    if (!match({TokenType::RETURN})) {
        return false;
    }

    vmWriter.write("// Compiling return");
    consume({TokenType::RETURN});

    if (!match({TokenType::SEMICOLON})) {
        zeroOrOnce([this] { return compileExpression(); });
    } else {
        vmWriter.writePush(Segment::CONST, 0);
    }

    vmWriter.writeReturn();

    consume({TokenType::SEMICOLON});

    return true;
};

bool CompilationEngine::compileExpression() {
    // term (op term)*

    compileTerm();

    zeroOrMany([this] {
        if (match(ops)) {
            Token op = consume(ops);
            compileTerm();
            vmWriter.write(opCommandMap.at(op));
            // token++;
            return true;
        }
        return false;
    });

    return true;
};

bool CompilationEngine::compileTerm() {
    // integerConstant | stringConstant | keywordConstant | varName |
    // varName '[' expression ']' | subroutineCall |
    // '(' expression ')' | unaryOp term

    oneOf([this] { return compileIntConst(); },
          [this] { return compileStringConst(); },
          [this] { return compileKeywordConstant(); },
          [this] {
              if (match({TokenType::LPAREN})) {
                  consume({TokenType::LPAREN});
                  compileExpression();
                  consume({TokenType::RPAREN});
                  return true;
              }
              return false;
          },
          [this] { return compileUnaryOp(); },
          [this] {
              if (!match({TokenType::IDENTIFIER})) {
                  return false;
              }
              Token ident = consume({TokenType::IDENTIFIER});

              if (matchNext({TokenType::FULL_STOP, TokenType::LPAREN})) {
                  // TODO - do I need to pass in ident?
                  return compileSubroutineCall();
              }

              Symbol symbol = symbolTable.getSymbol(ident);
              auto segment = kindSegmentMap.at(symbol.kind);

              token++; // ??
              if (match({TokenType::LBRACKET})) {
                  consume({TokenType::LBRACKET});
                  vmWriter.writePush(segment, symbol.id);
                  compileExpression();
                  consume({TokenType::RBRACKET});
                  vmWriter.write("add");
                  vmWriter.writePop(Segment::POINTER, 1);
                  vmWriter.writePush(Segment::THAT, 0);

                  return true;
              }

              vmWriter.writePush(segment, symbol.id);
              return true;
          });

    return true;
};

bool CompilationEngine::compileSubroutineCall() {
    // subroutineName '(' expressionList ')' | (className | varName)
    // '.' subroutineName '(' expressionList ')'

    std::string typeName{};
    std::string name{};
    int numArgs = 0;

    // TODO increment num args correctly throughout
    if (matchNext({TokenType::FULL_STOP})) {
        Token ident = consume({TokenType::IDENTIFIER});
        consume({TokenType::FULL_STOP});

        Symbol symbol = symbolTable.getSymbol(ident);

        if (symbol.get() == nullptr) {
            // it's a class
            typeName = ident.src;
        } else {
            typeName = symbol.type;
            auto segment = kindSegmentMap.at(symbol.kind);
            numArgs += 1;
            vmWriter.writePush(segment, symbol.id);
        }
        Token methodName = consume({TokenType::IDENTIFIER});
        name = typeName + "." + methodName.src;

    } else {
        Token ident = consume({TokenType::IDENTIFIER});
        numArgs = 1;
        vmWriter.writePush(Segment::POINTER, 0);
        name = className + "." + ident.src;
    }

    consume({TokenType::LPAREN});
    if (!match({TokenType::RPAREN})) {
        numArgs += compileExpressionList();
    }

    consume({TokenType::RPAREN});

    vmWriter.writeCall(name, numArgs);

    return true;
};

int CompilationEngine::compileExpressionList() {
    // (expression (',' expression)* )?

    int numArgs = 0;
    compileExpression();
    numArgs++;
    while (match({TokenType::COMMA})) {
        token++;
        compileExpression();
        numArgs++;
    }
    return numArgs;
};

bool CompilationEngine::compileUnaryOp() {
    // '-' | '~' term

    Token op = consume({TokenType::TILDE, TokenType::HYPHEN});
    compileTerm();
    vmWriter.write(unaryOpCommandMap.at(op.tokenType));
    return true;
};

bool CompilationEngine::compileKeywordConstant() {
    // 'true'| 'false' | 'null' | 'this'

    Token keyword = consume({TokenType::TRUE, TokenType::FALSE,
                             TokenType::COMP_NULL, TokenType::THIS});
    if (keyword.tokenType == TokenType::TRUE) {
        vmWriter.writePush(Segment::CONST, 1);
        vmWriter.write("neg");
    } else if (keyword.tokenType == TokenType::FALSE ||
               keyword.tokenType == TokenType::COMP_NULL) {
        vmWriter.writePush(Segment::CONST, 0);
    } else {
        vmWriter.writePush(Segment::POINTER, 0);
    }
    return true;
};

bool CompilationEngine::compileIntConst() {
    Token intToken = consume({TokenType::INTEGER});

    vmWriter.writePush(Segment::CONST, std::stoi(intToken.src));

    return true;
};

bool CompilationEngine::compileStringConst() {
    Token strToken = consume({TokenType::STRING});

    auto string = strToken.src;
    vmWriter.writePush(Segment::CONST, string.length());
    vmWriter.writeCall("String.new", 1);
    for (const char &c : string) {
        vmWriter.writePush(Segment::CONST, int(c));
        vmWriter.writeCall("String.appendChar", 2);
    }
    token++;

    return true;
};

// Private helper methods
// ======================

Token CompilationEngine::readType() {
    // 'int' | 'char' | 'boolean' | className

    return oneOfToken(
        [this] {
            return consume(
                {TokenType::INT, TokenType::CHAR, TokenType::BOOLEAN});
        },
        [this] { return consume({TokenType::IDENTIFIER}); });
};

Token CompilationEngine::consume(MatchOptions options) {
    Token curr = *token;

    for (const TokenType &option : options) {
        if (curr.tokenType == option) {
            token++;
            return curr;
        }
    }

    throw CompilationError(expected(options, *token));
};

bool CompilationEngine::match(MatchOptions options) {
    return std::find(std::begin(options), std::end(options),
                     (*token).tokenType) != std::end(options);
};

bool CompilationEngine::matchNext(MatchOptions options) {
    auto next = std::next(token);
    return std::find(std::begin(options), std::end(options),
                     (*next).tokenType) != std::end(options);
};

bool CompilationEngine::zeroOrOnce(const std::function<void(void)> &F) {
    try {
        F();
        return true;
    } catch (const CompilationError &e) {
        return false;
    }
};

bool CompilationEngine::zeroOrMany(const std::function<bool(void)> &F) {
    try {
        while (F()) {
        }
        return true;
    } catch (const CompilationError &e) {
        return false;
    }
};

const std::string CompilationEngine::expected(MatchOptions options,
                                              const Token &got) {
    // TODO: stringify options
    std::stringstream ss{};
    ss << "l" << got.lineNumber << ": expected " << expect << ", received '"
       << got.src << "'" << std::endl;
    return ss.str();
};

const std::string CompilationEngine::newLabel() {
    return className + ".label." + std::to_string(labelCount++);
};

#include "CompilationEngine.hpp"

const std::map<SymbolKind::Enum, Segment::Enum> kindSegmentMap = {
    {SymbolKind::STATIC, Segment::STATIC},
    {SymbolKind::FIELD, Segment::THIS},
    {SymbolKind::ARGUMENT, Segment::ARG},
    {SymbolKind::VAR, Segment::LOCAL},
};

const std::map<TokenType, std::string> opCommandMap = {
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

const std::map<TokenType, std::string> unaryOpCommandMap = {
    {TokenType::HYPHEN, "neg"}, {TokenType::TILDE, "not"}};

MatchOptions ops = {
    TokenType::PLUS,          TokenType::HYPHEN,       TokenType::STAR,
    TokenType::FORWARD_SLASH, TokenType::AMPERSAND,    TokenType::BAR,
    TokenType::LESS_THAN,     TokenType::GREATER_THAN, TokenType::EQUALS};

MatchOptions keywords = {TokenType::COMP_TRUE, TokenType::COMP_FALSE,
                         TokenType::COMP_NULL, TokenType::THIS};

CompilationEngine::CompilationEngine(TokenList &tokens, std::ostream &out)
    : token{tokens.begin()}, vmWriter{out}, symbolTable{}, labelCount{0} {}

// Public compilation methods
// ==========================

bool CompilationEngine::compile() {
    try {
        compileClass();
        return true;
    } catch (const CompilationError &e) {
        std::cerr << "Compilation error:\n" << e.what() << std::endl;
        return false;
    } catch (const SymbolNotFoundError &e) {
        std::cerr << "Compilation error:\n" << e.what() << std::endl;
        return false;
    }
};

void CompilationEngine::compileClass() {
    // 'class' className '{' classVarDec* subroutineDec* '}'

    consume({TokenType::CLASS});
    Token ident = consume({TokenType::IDENTIFIER});

    className = ident.src;

    consume({TokenType::LBRACE});

    while (match({TokenType::STATIC, TokenType::FIELD})) {
        compileClassVarDec();
    }
    while (match(
        {TokenType::CONSTRUCTOR, TokenType::FUNCTION, TokenType::METHOD})) {
        compileSubroutineDec();
    }
};

void CompilationEngine::compileClassVarDec() {
    // ('static' | 'field' ) type varName (',' varName)* ';'

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
};

void CompilationEngine::compileSubroutineDec() {
    // ('constructor' | 'function' | 'method')
    // ('void' | type) subroutineName '(' parameterList ')'
    // subroutineBody

    symbolTable.startSubroutine();
    vmWriter.write("// Compiling subroutine");

    Token keyword = consume(
        {TokenType::CONSTRUCTOR, TokenType::FUNCTION, TokenType::METHOD});
    if (keyword.tokenType == TokenType::METHOD) {
        symbolTable.addSymbol("this", className, SymbolKind::ARGUMENT);
    }

    Token type;
    if (match({TokenType::VOID})) {
        type = consume({TokenType::VOID});
    } else {
        type = readType();
    }
    Token ident = consume({TokenType::IDENTIFIER});

    compileParameterList();

    // TODO add type
    compileSubroutineBody(ident, keyword);
    vmWriter.write("// End subroutine");
};

void CompilationEngine::compileParameterList() {
    // ((type varName) (',' type varName)*)?

    consume({TokenType::LPAREN});

    if (match({TokenType::RPAREN})) {
        consume({TokenType::RPAREN});
        return;
    }

    Token type = readType();
    Token ident = consume({TokenType::IDENTIFIER});

    symbolTable.addSymbol(ident, type, SymbolKind::ARGUMENT);

    while (match({TokenType::COMMA})) {
        consume({TokenType::COMMA});
        Token type = readType();
        Token ident = consume({TokenType::IDENTIFIER});

        symbolTable.addSymbol(ident, type, SymbolKind::ARGUMENT);
    }

    consume({TokenType::RPAREN});
};

void CompilationEngine::compileVarDec() {
    // 'var' type varName (',' varName)* ';'

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
};

void CompilationEngine::compileSubroutineBody(const Token &name,
                                              const Token &keyword) {
    // '{' varDec* statements '}'

    vmWriter.write("// Compiling subroutine body");
    consume({TokenType::LBRACE});

    while (match({TokenType::VAR})) {
        compileVarDec();
    }

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
};

void CompilationEngine::compileStatements() {
    // statement*

    while (match({TokenType::LET, TokenType::IF, TokenType::ELSE,
                  TokenType::WHILE, TokenType::DO, TokenType::RETURN})) {
        compileStatement();
    }
};

void CompilationEngine::compileStatement() {
    // letStatement | ifStatement | whileStatement | doStatement |
    // returnStatement

    if (match({TokenType::LET})) {
        compileLet();
    } else if (match({TokenType::IF})) {
        compileIf();
    } else if (match({TokenType::WHILE})) {
        compileWhile();
    } else if (match({TokenType::DO})) {
        compileDo();
    } else if (match({TokenType::RETURN})) {
        compileReturn();
    } else {
        throw CompilationError{
            expected({TokenType::LET, TokenType::IF, TokenType::WHILE,
                      TokenType::DO, TokenType::RETURN})};
    }
};

void CompilationEngine::compileLet() {
    // 'let' varName ('[' expression ']')? '=' expression ';'

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
};

void CompilationEngine::compileIf() {
    // 'if '(' expression ')' '{' statements '}' ('else' '{' statements
    // '}')?

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
};

void CompilationEngine::compileWhile() {
    // 'while' '(' expression ')' '{' statements '}'

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
};

void CompilationEngine::compileDo() {
    // 'do' subroutineCall ';'

    vmWriter.write("// Compiling do");
    consume({TokenType::DO});

    compileSubroutineCall();

    consume({TokenType::SEMICOLON});
    vmWriter.writePop(Segment::TEMP, 0);
};

void CompilationEngine::compileReturn() {
    // 'return' expression? ';'

    vmWriter.write("// Compiling return");
    consume({TokenType::RETURN});

    if (match({TokenType::SEMICOLON})) {
        vmWriter.writePush(Segment::CONST, 0);
    } else {
        compileExpression();
    }

    vmWriter.writeReturn();

    consume({TokenType::SEMICOLON});
};

void CompilationEngine::compileExpression() {
    // term (op term)*

    compileTerm();

    while (match(ops)) {
        Token op = consume(ops);
        compileTerm();
        vmWriter.write(opCommandMap.at(op.tokenType));
    }
};

void CompilationEngine::compileTerm() {
    // integerConstant | stringConstant | keywordConstant | varName |
    // varName '[' expression ']' | subroutineCall |
    // '(' expression ')' | unaryOp term

    if (match({TokenType::INTEGER})) {
        compileIntConst();
    } else if (match({TokenType::STRING})) {
        compileStringConst();
    } else if (match(keywords)) {
        compileKeywordConstant();
    } else if (match({TokenType::LPAREN})) {
        consume({TokenType::LPAREN});
        compileExpression();
        consume({TokenType::RPAREN});
    } else if (match({TokenType::TILDE, TokenType::HYPHEN})) {
        compileUnaryOp();
    } else if (match({TokenType::IDENTIFIER})) {
        compileIdentifier();
    } else {
        throw CompilationError{expected(
            {TokenType::INTEGER, TokenType::STRING, TokenType::LPAREN,
             TokenType::TILDE, TokenType::HYPHEN, TokenType::IDENTIFIER})};
    }
};

void CompilationEngine::compileIdentifier() {
    if (matchNext({TokenType::FULL_STOP, TokenType::LPAREN})) {
        compileSubroutineCall();
        return;
    }
    Token ident = consume({TokenType::IDENTIFIER});

    Symbol symbol = symbolTable.getSymbol(ident);
    auto segment = kindSegmentMap.at(symbol.kind);
    if (match({TokenType::LBRACKET})) {
        compileArrayLiteral(segment, symbol);
        return;
    }
    vmWriter.writePush(segment, symbol.id);
};

void CompilationEngine::compileSubroutineCall() {
    // subroutineName '(' expressionList ')' | (className | varName)
    // '.' subroutineName '(' expressionList ')'

    std::string typeName{};
    std::string name{};
    int numArgs = 0;

    Token ident = consume({TokenType::IDENTIFIER});
    // TODO increment num args correctly throughout
    if (match({TokenType::FULL_STOP})) {
        consume({TokenType::FULL_STOP});

        try {
            Symbol symbol = symbolTable.getSymbol(ident);
            typeName = symbol.type;
            auto segment = kindSegmentMap.at(symbol.kind);
            numArgs += 1;
            vmWriter.writePush(segment, symbol.id);
        } catch (const SymbolNotFoundError &e) {
            // it's a class
            typeName = ident.src;
        }

        Token methodName = consume({TokenType::IDENTIFIER});
        name = typeName + "." + methodName.src;

    } else {
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
};

int CompilationEngine::compileExpressionList() {
    // (expression (',' expression)* )?

    int numArgs = 0;
    compileExpression();
    numArgs++;
    while (match({TokenType::COMMA})) {
        consume({TokenType::COMMA});
        compileExpression();
        numArgs++;
    }
    return numArgs;
};

void CompilationEngine::compileUnaryOp() {
    // '-' | '~' term

    Token op = consume({TokenType::TILDE, TokenType::HYPHEN});
    compileTerm();
    vmWriter.write(unaryOpCommandMap.at(op.tokenType));
};

void CompilationEngine::compileKeywordConstant() {
    // 'true'| 'false' | 'null' | 'this'

    Token keyword = consume(keywords);
    if (keyword.tokenType == TokenType::COMP_TRUE) {
        vmWriter.writePush(Segment::CONST, 1);
        vmWriter.write("neg");
    } else if (keyword.tokenType == TokenType::COMP_FALSE ||
               keyword.tokenType == TokenType::COMP_NULL) {
        vmWriter.writePush(Segment::CONST, 0);
    } else {
        vmWriter.writePush(Segment::POINTER, 0);
    }
};

void CompilationEngine::compileIntConst() {
    Token intToken = consume({TokenType::INTEGER});

    vmWriter.writePush(Segment::CONST, std::stoi(intToken.src));
};

void CompilationEngine::compileStringConst() {
    Token strToken = consume({TokenType::STRING});

    auto string = strToken.src;
    vmWriter.writePush(Segment::CONST, string.length());
    vmWriter.writeCall("String.new", 1);
    for (const char &c : string) {
        vmWriter.writePush(Segment::CONST, int(c));
        vmWriter.writeCall("String.appendChar", 2);
    }
};

void CompilationEngine::compileArrayLiteral(const Segment::Enum &segment,
                                            const Symbol &symbol) {
    consume({TokenType::LBRACKET});
    vmWriter.writePush(segment, symbol.id);
    compileExpression();
    consume({TokenType::RBRACKET});
    vmWriter.write("add");
    vmWriter.writePop(Segment::POINTER, 1);
    vmWriter.writePush(Segment::THAT, 0);
};

// Private helper methods
// ======================

Token CompilationEngine::readType() {
    // 'int' | 'char' | 'boolean' | className
    return consume({TokenType::INT, TokenType::CHAR, TokenType::BOOLEAN,
                    TokenType::IDENTIFIER});
};

Token CompilationEngine::consume(MatchOptions options) {
    Token curr = *token;

    for (const TokenType &option : options) {
        if (curr.tokenType == option) {
            token++;
            return curr;
        }
    }

    throw CompilationError{expected(options)};
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

const std::string CompilationEngine::expected(MatchOptions options) {
    std::vector<std::string> opts;
    std::transform(options.begin(), options.end(), std::back_inserter(opts),
                   [](const TokenType &opt) { return tokenString(opt); });
    const std::string optList = boost::algorithm::join(opts, ", ");

    std::stringstream ss{};
    ss << "Line " << (*token).lineNumber << ": expected '" << optList << "'. "
       << "But received '" << (*token).src << "'" << std::endl;
    return ss.str();
};

const std::string CompilationEngine::newLabel() {
    return className + ".label." + std::to_string(labelCount++);
};

#ifndef __Tokens__
#define __Tokens__

#include <string>

class Token {
  public:
    Token(int line) : lineNumber(line){};
    virtual ~Token() = default;
    [[nodiscard]] virtual std::string toString() const { return ""; };
    [[nodiscard]] virtual std::string valToString() const { return ""; };
    [[nodiscard]] int getLineNumber() const noexcept { return lineNumber; };
    template <typename T> T *instance() { return dynamic_cast<T *>(this); }

  protected:
    int lineNumber;
};

class KeywordToken : public Token {
  public:
    KeywordToken(std::string kw, int line) : Token(line), val(std::move(kw)){};
    [[nodiscard]] std::string getVal() const noexcept { return val; };
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] std::string valToString() const override { return val; };

  private:
    std::string val;
};

class SymbolToken : public Token {
  public:
    SymbolToken(char16_t symbol, int line) : Token(line), val(symbol){};
    [[nodiscard]] char16_t getVal() const noexcept { return val; };
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] std::string valToString() const override;

  private:
    char16_t val;
};

class IntConstToken : public Token {
  public:
    IntConstToken(int16_t intVal, int line) : Token(line), val(intVal){};
    [[nodiscard]] int16_t getVal() const noexcept { return val; };
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] std::string valToString() const override;

  private:
    int16_t val;
};

class StringToken : public Token {
  public:
    StringToken(std::string stringVal, int line)
        : Token(line), val(std::move(stringVal)){};
    [[nodiscard]] std::string getVal() const noexcept { return val; };
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] std::string valToString() const override { return val; };

  private:
    std::string val;
};

class IdentifierToken : public Token {
  public:
    IdentifierToken(std::string val, int line)
        : Token(line), val(std::move(val)){};
    [[nodiscard]] std::string getVal() const noexcept { return val; };
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] std::string valToString() const override { return val; };

  private:
    std::string val;
};

#endif

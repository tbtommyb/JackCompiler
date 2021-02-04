#ifndef __code__
#define __code__

#include "Parser.hpp"
#include "SymbolTable.hpp"
#include <string>

class Code {
    enum CodeSize : int { Dest = 3, Comp = 7, Val = 15 };

  public:
    Code(Instruction &instr, SymbolTable &mapping);
    ~Code() = default;
    [[nodiscard]] const std::string string() const;

  private:
    SymbolTable &mappings;
    Instruction instruction;
    std::bitset<Dest> dest;
    std::bitset<Comp> comp;
    std::bitset<Dest> jump;
    std::bitset<Val> value;
};

#endif

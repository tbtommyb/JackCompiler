#ifndef __code__
#define __code__

#include "Parser.hpp"
#include "SymbolTable.hpp"
#include <string>

class Code {
  public:
    Code(Instruction &instr, SymbolTable &mapping);
    ~Code() = default;
    const std::string string() const;

  private:
    SymbolTable &mappings;
    Instruction instruction;
    std::bitset<3> dest;
    std::bitset<7> comp;
    std::bitset<3> jump;
    std::bitset<15> value;
};

#endif

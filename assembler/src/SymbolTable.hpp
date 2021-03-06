#ifndef __symbol_table__
#define __symbol_table__

#include <map>
#include <string>

class SymbolTable {
  public:
    SymbolTable();
    ~SymbolTable() = default;
    void addEntry(std::string, unsigned int);
    void addVariable(std::string);
    bool contains(std::string);
    unsigned int getAddress(std::string);

  private:
    std::map<std::string, unsigned int> mapping;
    unsigned int currentVariableAddress;
};

#endif

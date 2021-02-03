#ifndef __assemblr__
#define __assemblr__

#include "Code.hpp"
#include "Parser.hpp"
#include "SymbolTable.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Assemblr {
  public:
    Assemblr(std::istream &program, std::ostream &out);
    void run();

  private:
    void buildSymbolTable(SymbolTable &table, Parser &parser);
    std::ostream &out;

    std::istream &program;
    SymbolTable symbols;
    Parser parser;
};

#endif

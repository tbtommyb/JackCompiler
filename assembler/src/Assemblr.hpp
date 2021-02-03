#ifndef __assemblr__
#define __assemblr__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Parser.hpp"
#include "Code.hpp"
#include "SymbolTable.hpp"

void Assemble(std::istream& program, std::ostream& out);
void buildSymbolTable(SymbolTable& table, Parser& parser);
std::string getFilename(std::string input);

#endif

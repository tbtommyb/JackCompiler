#ifndef __Compiler__
#define __Compiler__

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include "CompilationEngine.hpp"
#include "JackTokenizer.hpp"
#include "SymbolTable.hpp"

namespace fs = boost::filesystem;

class Compiler {
  public:
    Compiler() = default;
    ~Compiler() = default;
    void run(std::string dir);
    bool process(const std::string &fileName, std::istream &program, std::ostream &out);

  private:
    std::vector<fs::path> filesToProcess;
};

#endif

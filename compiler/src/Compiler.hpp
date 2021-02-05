#ifndef __Compiler__
#define __Compiler__

#include "CompilationEngine.hpp"
#include "JackTokenizer.hpp"
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

namespace fs = boost::filesystem;

class Compiler {
  public:
    Compiler() = default;
    ~Compiler() = default;
    void run(std::string dir);
    void process(std::istream &program, std::ostream &out);

  private:
    std::vector<fs::path> filesToProcess;
};

#endif

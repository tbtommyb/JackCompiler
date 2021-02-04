#ifndef __VM__
#define __VM__

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include "CodeWriter.hpp"
#include "Parser.hpp"

namespace fs = boost::filesystem;

class VM {
  public:
    VM(std::ostream &output);
    void run(fs::path &input);
    void process(std::istream &input, std::string inputName);

  private:
    CodeWriter writer;
};
#endif

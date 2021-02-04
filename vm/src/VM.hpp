#ifndef __VM__
#define __VM__

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include "CodeWriter.hpp"
#include "Parser.hpp"

class VM {
  public:
    VM(CodeWriter &writer);
    void process(std::istream &input, std::string inputName);

  private:
    CodeWriter &writer;
};
#endif

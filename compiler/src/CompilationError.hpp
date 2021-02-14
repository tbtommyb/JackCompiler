#ifndef __CompilationError__
#define __CompilationError__

#include <string>
#include <stdexcept>

class CompilationError : public std::runtime_error {
  public:
    CompilationError(std::string msg) : std::runtime_error{msg} {}
};

#endif

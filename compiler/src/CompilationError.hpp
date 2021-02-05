#ifndef __CompilationError__
#define __CompilationError__

#include <exception>
#include <string>

class CompilationError : public std::exception {
  public:
    CompilationError(const char *msg) : msg(msg) {}
    CompilationError(std::string msg) : CompilationError(msg.c_str()) {}
    [[nodiscard]] const char *what() const noexcept override { return msg; }

  private:
    const char *msg;
};

#endif

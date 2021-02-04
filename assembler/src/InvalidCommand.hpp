#include <exception>
#include <string>

class InvalidCommand : public std::exception {
  public:
    InvalidCommand(std::string &command) : command(std::move(command)) {}
    ~InvalidCommand() noexcept override = default;
    [[nodiscard]] const char *what() const noexcept override {
        return command.c_str();
    }

  private:
    std::string command;
};

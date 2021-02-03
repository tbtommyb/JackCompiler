#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <fstream>
#include <string>
#include <sstream>

#include "Assemblr.hpp"

TEST_CASE("Expected .hack output", "[Assemble]") {
    std::ifstream testInput{"test/inputs/Add.asm"};
    std::ifstream expected{"test/inputs/Add.hack"};
    std::string content( (std::istreambuf_iterator<char>(expected) ),
                         (std::istreambuf_iterator<char>()) );
    std::stringstream output;
    Assemble(testInput, output);
    REQUIRE(output.str() == content);
}

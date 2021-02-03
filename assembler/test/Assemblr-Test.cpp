#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <fstream>
#include <sstream>
#include <string>

#include "Assemblr.hpp"

TEST_CASE("Expected .hack output", "[Assemble]") {
    std::ifstream testInput{"../test/inputs/Add.asm"};
    std::ifstream expected{"../test/inputs/Add.hack"};
    std::string content((std::istreambuf_iterator<char>(expected)),
                        (std::istreambuf_iterator<char>()));
    std::stringstream output;
    Assemblr *assemblr = new Assemblr{testInput, output};
    assemblr->run();
    REQUIRE(output.str() == content);
}

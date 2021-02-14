#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <fstream>
#include <sstream>
#include <string>

#include "Compiler.hpp"

TEST_CASE("Expected .vm output", "[Compiler]") {
    std::string testCases[]{"Average", "ComplexArrays", "ConvertToBin", "Seven"};

    for (auto const &testCase : testCases) {
        SECTION(testCase) {
            std::ifstream testInput{"../test/inputs/" + testCase +
                                    "/Main.jack"};
            std::ifstream expected{"../test/inputs/" + testCase + "/Main.vm"};

            assert(testInput.is_open());
            assert(expected.is_open());

            std::stringstream output;
            std::stringstream content;
            content << expected.rdbuf();

            Compiler{}.process(testCase, testInput, output);
            testInput.close();
            expected.close();

            std::string out = output.str();
            std::string exp = content.str();
            for (std::string::size_type i = 0; i < out.size(); ++i) {
                if (out[i] != exp[i]) {
                    std::cout << "at :" << i << " " << std::hex << (int)out[i]
                              << " does not match " << std::hex << (int)exp[i]
                              << std::endl;
                    REQUIRE(false);
                }
            }
            REQUIRE(output.str() == content.str());
        }
    }
}

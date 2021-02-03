cmake_minimum_required(VERSION 3.5)
project (Assemblr)

file(GLOB_RECURSE sources src/*.cpp src/*.hpp)
file(GLOB_RECURSE test_sources test/*.cpp)

add_executable(assemblr ${sources} main.cpp)
target_compile_options(assemblr PUBLIC -std=c++17 -Wall)

find_package(Catch2 REQUIRED)
add_executable(tests ${test_sources} ${sources})
target_link_libraries(tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE src)

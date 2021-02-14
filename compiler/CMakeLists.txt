cmake_minimum_required(VERSION 3.5)
project (compiler)

set (CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE sources src/*.cpp src/*.hpp)
file(GLOB_RECURSE test_sources test/*.cpp)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.70.0 COMPONENTS filesystem)

if(Boost_FOUND)
    add_executable(compiler ${sources} main.cpp)
    target_link_libraries(compiler Boost::filesystem)
endif()

target_compile_options(compiler PUBLIC -g -std=c++17 -Wall -Wextra -pedantic)

find_package(Catch2 REQUIRED)
add_executable(tests ${test_sources} ${sources})
target_link_libraries(tests PRIVATE Boost::filesystem Catch2::Catch2 Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE src)

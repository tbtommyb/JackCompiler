cmake_minimum_required(VERSION 3.5)
project (vm)

file(GLOB_RECURSE sources src/*.cpp src/*.hpp)
file(GLOB_RECURSE test_sources test/*.cpp)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.70.0 COMPONENTS filesystem)

if(Boost_FOUND)
    add_executable(vm ${sources} main.cpp)
    target_link_libraries(vm Boost::filesystem)
endif()

target_compile_options(vm PUBLIC -std=c++17 -Wall -Wextra -pedantic)

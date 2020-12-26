cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")

project(colex)

set(SRC iterators.hpp expressions.hpp colex.hpp)
set(TEST_SRC doctest.cpp doctest.hpp)

add_library(colex SHARED ${SRC})
set_target_properties(colex PROPERTIES LINKER_LANGUAGE CXX)

add_executable(tests ${SRC} ${TEST_SRC})

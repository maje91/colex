cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")

project(colex)

set(INC iterators.hpp expressions.hpp colex.hpp)
set(SRC colex.cpp)
set(TEST_SRC doctest.cpp doctest.hpp)

add_library(colex SHARED ${INC} ${SRC})

add_executable(tests ${INC} ${SRC} ${TEST_SRC})

install(TARGETS colex DESTINATION lib)
install(FILES ${INC} DESTINATION include)

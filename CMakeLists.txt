cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")

project(colex)

set(LIB_NAME ${PROJECT_NAME})
set(TESTS_NAME ${PROJECT_NAME}_tests)

set(INC iterators.hpp expressions.hpp colex.hpp iterator_interface.hpp expression_interface.hpp)
set(SRC colex.cpp)
set(TEST_SRC doctest.cpp doctest.hpp)

add_library(${LIB_NAME} SHARED ${INC} ${SRC})

add_executable(${TESTS_NAME} ${INC} ${SRC} ${TEST_SRC})

install(TARGETS ${LIB_NAME} DESTINATION lib)
install(FILES ${INC} DESTINATION include)

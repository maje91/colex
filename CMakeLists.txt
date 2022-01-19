cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")

project(colex)

set(LIB_NAME ${PROJECT_NAME})
set(TESTS_NAME ${PROJECT_NAME}_tests)

set(ITERATORS_SRC
        iterators/inc/iterators.hpp
        iterators/inc/interface.hpp
        iterators/src/map.hpp
        iterators/src/stl.hpp
        iterators/src/pointer.hpp
        iterators/src/scan.hpp
        iterators/src/filter.hpp
        iterators/src/flat_map.hpp
        iterators/src/flatten.hpp
        iterators/src/take.hpp
        iterators/src/drop.hpp
        iterators/src/enumerate.hpp
        iterators/src/window.hpp
        iterators/src/range.hpp
        iterators/src/function.hpp
        iterators/src/concat.hpp
        iterators/src/chunk_map.hpp
        iterators/src/chunk.hpp
        iterators/src/partition.hpp
        iterators/src/partition_map.hpp
        iterators/src/zip.hpp)

set(EXPRESSIONS_SRC
        expressions/inc/expressions.hpp
        expressions/inc/interface.hpp
        expressions/src/map.hpp
        expressions/src/filter.hpp
        expressions/src/fold.hpp
        expressions/src/scan.hpp
        expressions/src/flat_map.hpp
        expressions/src/flatten.hpp
        expressions/src/window.hpp
        expressions/src/take.hpp
        expressions/src/drop.hpp
        expressions/src/enumerate.hpp
        expressions/src/chunk_map.hpp
        expressions/src/chunk.hpp
        expressions/src/partition.hpp
        expressions/src/partition_map.hpp
        expressions/src/prepend.hpp
        expressions/src/append.hpp
        expressions/src/for_each.hpp
        expressions/src/composition.hpp)

set(ROOT_SRC colex.cpp colex.hpp)

set(TEST_SRC tests.cpp doctest.hpp)

set(SRC ${ROOT_SRC} ${EXPRESSIONS_SRC} ${ITERATORS_SRC})

add_library(${LIB_NAME} SHARED ${SRC})
target_include_directories(${LIB_NAME} PRIVATE .)

add_executable(${TESTS_NAME} ${SRC} ${TEST_SRC})
target_include_directories(${TESTS_NAME} PRIVATE .)


install(TARGETS ${LIB_NAME} DESTINATION lib)
install(FILES ${INC} DESTINATION include/${LIB_NAME})

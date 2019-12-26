cmake_minimum_required(VERSION 3.15)
project(Earley)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(
        googletest-master
)

add_executable(Earley main.cpp Grammar.cpp Grammar.h EarleyAlgorithm.cpp EarleyAlgorithm.h)

add_executable(unit_tests test.cpp tests.cpp EarleyAlgorithm.cpp Grammar.cpp)
target_include_directories(
        unit_tests
        PUBLIC
        gtest
        gtest_main
)

target_link_libraries(
        unit_tests
        PUBLIC
        gtest
        gtest_main
)

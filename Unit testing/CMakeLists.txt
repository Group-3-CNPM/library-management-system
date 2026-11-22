cmake_minimum_required(VERSION 3.16)
project(LibraryManagementTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

add_executable(run_unit_tests
    test_core_methods.cpp
)

target_link_libraries(run_unit_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(run_unit_tests)
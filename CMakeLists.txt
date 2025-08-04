cmake_minimum_required(VERSION 3.18)

project(like-a-chess-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

add_subdirectory(pybind11)
add_subdirectory(chess_library_src)
add_subdirectory(chess_board_tests)

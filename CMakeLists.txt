cmake_minimum_required(VERSION 3.18)

project(chess-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(chess_board)

add_executable(
    test
    test.cpp
)

target_link_libraries(
    test
    chess_board
)
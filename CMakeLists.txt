cmake_minimum_required(VERSION 3.18)

project(like-a-chess-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Curses REQUIRED)

add_subdirectory(chess_library_src)

set(Mode "" CACHE STRING "Choose what to build: CLI (cli) chess loading, unit tests (tests) or Python library (python)")

if (Mode STREQUAL "tests")
    add_subdirectory(chess_board_tests)
elseif(Mode STREQUAL "cli")
    add_subdirectory(chess_cli)
else ()
    message(WARNING ${Mode})
    message(WARNING "No valid -DMode option provided (use 'cli', 'tests', or 'python'); or mode is not implemented yet.")
endif()

add_executable(
    unit_tests
    unit_tests.cpp
    board_tests.cpp
    move_tests.cpp
)

target_include_directories(
    unit_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/chess_board
)

target_link_libraries(
    unit_tests
    chess_library_src
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

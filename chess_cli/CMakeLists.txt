
add_subdirectory(cli)
add_executable(main main.cpp)

target_link_libraries(
    main
    PRIVATE
        chess_library_src
        cli
)

target_include_directories(
    main
    PRIVATE
        ${CMAKE_SOURCE_DIR}/chess_library_src
)

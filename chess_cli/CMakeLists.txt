# Build the components first
add_subdirectory(cli)
add_subdirectory(chess_app)

# Create the main executable
add_executable(main main.cpp)

# Link against the built libraries
target_link_libraries(main
    PRIVATE
        chess_library_src
        chess_app
        window
        resources
        chess_app_builder
        builder
        app
)

# Add include directories needed by main
target_include_directories(main
    PRIVATE
        ${CMAKE_SOURCE_DIR}/chess_cli/cli/components/app
        ${CMAKE_SOURCE_DIR}/chess_cli/chess_app/builder  # For chess_app_builder.h
)
cmake_minimum_required(VERSION 3.10)

# Find ncurses package
find_package(Curses REQUIRED)

# Create the app library
add_library(app STATIC
    app.cpp
    app.h
)

target_include_directories(app
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../resources
        ${CURSES_INCLUDE_DIR}
)

target_link_libraries(app
    PUBLIC
        ${CURSES_LIBRARIES}
)
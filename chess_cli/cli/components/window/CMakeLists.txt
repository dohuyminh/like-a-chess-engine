# Find ncurses package
find_package(Curses REQUIRED)

add_library(window STATIC
    window.cpp
    window.h
)

target_include_directories(window
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CURSES_INCLUDE_DIR}
)

target_link_libraries(window
    PUBLIC
        ${CURSES_LIBRARIES}
)

add_library(app_page STATIC
    chess_page.cpp
    chess_page.h
)

target_include_directories(app_page
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_SOURCE_DIR}/chess_cli/cli/components/window
)

target_link_libraries(app_page
    PRIVATE
        chess_library_src
        window
)

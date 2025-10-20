# This file must be linked to "app" library from chess_cli/cli/components/cli/CMakeLists.txt
add_library(chess_app STATIC
    chess_app.cpp
    chess_app.h
)

target_link_libraries(chess_app
    PRIVATE 
        chess_library_src 
        app_page
        resources
)

target_include_directories(chess_app 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/chess_cli/cli/components/app
)
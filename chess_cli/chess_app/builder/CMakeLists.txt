add_library(
    chess_app_builder STATIC
        chess_app_builder.h
        chess_app_builder.cpp 
)

target_link_libraries(
    chess_app_builder
    PRIVATE
        chess_library_src
        builder
        chess_app
)

target_include_directories(chess_app_builder 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/chess_cli/chess_app/components/app
        ${CMAKE_SOURCE_DIR}/chess_cli/chess_app/components/page
        ${CMAKE_SOURCE_DIR}/chess_cli/cli/components/window
)
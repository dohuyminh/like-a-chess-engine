add_library(chess_board "")

target_sources(
    chess_board 
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}/chess_piece.h
        ${CMAKE_CURRENT_LIST_DIR}/chess_board.h 
        ${CMAKE_CURRENT_LIST_DIR}/chess_board.cpp
        ${CMAKE_CURRENT_LIST_DIR}/chess_board_check.h 
        ${CMAKE_CURRENT_LIST_DIR}/chess_board_check.cpp
        ${CMAKE_CURRENT_LIST_DIR}/coord2D.h
        ${CMAKE_CURRENT_LIST_DIR}/coord2D.cpp 
        ${CMAKE_CURRENT_LIST_DIR}/move.h
        ${CMAKE_CURRENT_LIST_DIR}/move.cpp 
)

target_include_directories(
    chess_board
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)   

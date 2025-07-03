add_library(chess_board "")

target_sources(
    chess_board 
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}/chess_piece.h
        ${CMAKE_CURRENT_LIST_DIR}/chess_board.h 
        ${CMAKE_CURRENT_LIST_DIR}/chess_board.cpp
        # ${CMAKE_CURRENT_LIST_DIR}/board_factory.cpp 
        # ${CMAKE_CURRENT_LIST_DIR}/board_factory.h
)

target_include_directories(
    chess_board
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)   
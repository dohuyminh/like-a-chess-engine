add_library(chess_library_src "")

target_sources(
    chess_library_src
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}/chess_piece.h
        ${CMAKE_CURRENT_LIST_DIR}/chess_board.h 
        ${CMAKE_CURRENT_LIST_DIR}/chess_board.cpp
        ${CMAKE_CURRENT_LIST_DIR}/coord2D.h
        ${CMAKE_CURRENT_LIST_DIR}/coord2D.cpp 
        ${CMAKE_CURRENT_LIST_DIR}/utility.h
        ${CMAKE_CURRENT_LIST_DIR}/arena.h 
        ${CMAKE_CURRENT_LIST_DIR}/arena.cpp

        # square iterator
        ${CMAKE_CURRENT_LIST_DIR}/square_iterator/reachability.h
        ${CMAKE_CURRENT_LIST_DIR}/square_iterator/r_bishop.h
        ${CMAKE_CURRENT_LIST_DIR}/square_iterator/r_bishop.cpp 
        ${CMAKE_CURRENT_LIST_DIR}/square_iterator/r_composite.h
        ${CMAKE_CURRENT_LIST_DIR}/square_iterator/r_composite.cpp 
        ${CMAKE_CURRENT_LIST_DIR}/square_iterator/r_kingknight.h 
        ${CMAKE_CURRENT_LIST_DIR}/square_iterator/r_kingknight.cpp 
        ${CMAKE_CURRENT_LIST_DIR}/square_iterator/r_pawn.h
        ${CMAKE_CURRENT_LIST_DIR}/square_iterator/r_pawn.cpp 
        ${CMAKE_CURRENT_LIST_DIR}/square_iterator/r_rook.h
        ${CMAKE_CURRENT_LIST_DIR}/square_iterator/r_rook.cpp 
        ${CMAKE_CURRENT_LIST_DIR}/square_iterator/next_square_iterator.h
        ${CMAKE_CURRENT_LIST_DIR}/square_iterator/next_square_iterator.cpp 
        
        # moves
        ${CMAKE_CURRENT_LIST_DIR}/moves/get_all_moves.h
        ${CMAKE_CURRENT_LIST_DIR}/moves/get_all_moves.cpp
        ${CMAKE_CURRENT_LIST_DIR}/moves/chess_move.h
        ${CMAKE_CURRENT_LIST_DIR}/moves/chess_move.cpp
        ${CMAKE_CURRENT_LIST_DIR}/moves/castling.h
        ${CMAKE_CURRENT_LIST_DIR}/moves/castling.cpp
        ${CMAKE_CURRENT_LIST_DIR}/moves/underpromotion.h
        ${CMAKE_CURRENT_LIST_DIR}/moves/underpromotion.cpp
        ${CMAKE_CURRENT_LIST_DIR}/moves/queens_move.h
        ${CMAKE_CURRENT_LIST_DIR}/moves/queens_move.cpp
        ${CMAKE_CURRENT_LIST_DIR}/moves/knights_move.h
        ${CMAKE_CURRENT_LIST_DIR}/moves/knights_move.cpp
        ${CMAKE_CURRENT_LIST_DIR}/moves/algebraic_notation.h
        ${CMAKE_CURRENT_LIST_DIR}/moves/queens_algebraic.h
        ${CMAKE_CURRENT_LIST_DIR}/moves/queens_algebraic.cpp
        ${CMAKE_CURRENT_LIST_DIR}/moves/knights_algebraic.h
        ${CMAKE_CURRENT_LIST_DIR}/moves/knights_algebraic.cpp
        ${CMAKE_CURRENT_LIST_DIR}/moves/underpromotion_algebraic.h
        ${CMAKE_CURRENT_LIST_DIR}/moves/underpromotion_algebraic.cpp
        ${CMAKE_CURRENT_LIST_DIR}/moves/castling_algebraic.h
        ${CMAKE_CURRENT_LIST_DIR}/moves/castling_algebraic.cpp
        ${CMAKE_CURRENT_LIST_DIR}/moves/next_board.h
        ${CMAKE_CURRENT_LIST_DIR}/moves/next_board.cpp
        ${CMAKE_CURRENT_LIST_DIR}/moves/lru_movegen.h 
        ${CMAKE_CURRENT_LIST_DIR}/moves/lru_movegen.cpp
        
        # check terminal
        ${CMAKE_CURRENT_LIST_DIR}/check_terminal/check.h
        ${CMAKE_CURRENT_LIST_DIR}/check_terminal/check.cpp
        ${CMAKE_CURRENT_LIST_DIR}/check_terminal/checkmate.h
        ${CMAKE_CURRENT_LIST_DIR}/check_terminal/checkmate.cpp
)

target_link_libraries(
    chess_library_src
    PUBLIC
        pybind11::module
)

pybind11_add_module(
    chess_board
        ${CMAKE_CURRENT_LIST_DIR}/chess_board.cpp
)

target_include_directories(
    chess_library_src
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)   

#include <iostream>
#include "chess_board.h"

int main() {

    ChessBoard board;
    std::cout << (std::string)board;

    // const std::optional<Coord2D>& be = board.blackEnpassant();
    
    std::cout << board.blackLeftCastling() << '\n';

    return 0;
}
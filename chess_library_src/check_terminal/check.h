#pragma once 

#include "../chess_board.h"

#include <unordered_set>

namespace CheckTerminal
{
    bool __pieceCanReachSquare(const ChessBoard& board, Coord2D pieceCoord, Coord2D target);

    std::unordered_set<Coord2D> kingIsChecked(const ChessBoard& board, bool kingIsWhite);
} // namespace CheckTerminal

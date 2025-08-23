#pragma once 

#include "../chess_board.h"

#include <unordered_set>

namespace CheckTerminal
{
    std::unordered_set<Coord2D> kingIsChecked(const ChessBoard& board, Color kingsColor);
} // namespace CheckTerminal

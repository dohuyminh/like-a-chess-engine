#pragma once

#include "chess_board.h"
#include "move.h"
#include <unordered_set>

namespace CheckTerminal {
    std::unordered_set<Coord2D> kingIsChecked(const ChessBoard& board, bool kingIsWhite);
    
    bool isCheckmate(const ChessBoard& board, bool kingIsWhite);
}

#pragma once

#include "../chess_board.h"

namespace CheckTerminal
{
    enum class MateStatus {
        NONE, CHECK, CHECKMATE
    };
    
    MateStatus isCheckmate(const ChessBoard& board, bool kingIsWhite);
} // namespace CheckTerminal
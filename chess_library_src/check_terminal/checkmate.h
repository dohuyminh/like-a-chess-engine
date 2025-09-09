#pragma once

#include "../chess_board.h"

namespace internal {

namespace CheckTerminal
{
    enum class MateStatus {
        NONE, CHECK, CHECKMATE
    };
    
    MateStatus isCheckmate(const ChessBoard& board, Color kingsColor, std::size_t numLegalMoves);
} // namespace CheckTerminal
    
} // namespace internal

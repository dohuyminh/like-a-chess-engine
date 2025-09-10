#pragma once

#include "../chess_board.h"

#include <utility>
#include <string>

struct MoveResult {
    // for logging move history
    const std::string notation;

    // for updating board state
    const ChessBoard nextBoard;

    // for updating moves without progress
    const bool capture;
    const bool pawnMoved;

    MoveResult(std::string notation, ChessBoard nextBoard, bool capture, bool pawnMoved) :
        notation(notation),
        nextBoard(nextBoard),
        capture(capture),
        pawnMoved(pawnMoved)
    {}
};

namespace internal {

class AlgebraicNotation {
public:
    virtual ~AlgebraicNotation() = default;

protected:
    virtual MoveResult performMove(const ChessBoard& board) = 0;         
};

}
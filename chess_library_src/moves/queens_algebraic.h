#pragma once

#include "algebraic_notation.h"
#include "queens_move.h"

namespace internal {

class QueensAlgebraic final : public AlgebraicNotation {
public:

    QueensAlgebraic(QueensMove mv) noexcept;

    MoveResult performMove(const ChessBoard& board) override;

private:
    QueensMove _mv;
};

}
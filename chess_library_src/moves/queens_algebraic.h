#pragma once

#include "algebraic_notation.h"
#include "queens_move.h"

class QueensAlgebraic final : public AlgebraicNotation {
public:

    QueensAlgebraic(QueensMove mv, std::string resolveAmbiguity) noexcept;

    MoveResult performMove(const ChessBoard& state) override;

private:
    QueensMove _mv;
    std::string _resolveAmbiguity;
};
#pragma once 

#include "algebraic_notation.h"
#include "knights_move.h"

namespace internal {

class KnightsAlgebraic final : public AlgebraicNotation {
public:
    
    KnightsAlgebraic(KnightsMove mv) noexcept;

    MoveResult performMove(const ChessBoard& board) override;

private:
    KnightsMove _mv;
};

}
#pragma once

#include "algebraic_notation.h"
#include "castling.h"

namespace internal {

class CastlingAlgebraic final : public AlgebraicNotation {
public:
    
    CastlingAlgebraic(Castling mv) noexcept;
    
    MoveResult performMove(const ChessBoard& board) override;

private:
    Castling _mv;
};

}
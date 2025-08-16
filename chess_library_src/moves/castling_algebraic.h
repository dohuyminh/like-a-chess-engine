#pragma once

#include "algebraic_notation.h"
#include "castling.h"

class CastlingAlgebraic final : public AlgebraicNotation {
public:
    
    CastlingAlgebraic(Castling mv) noexcept;
    
    MoveResult performMove(const ChessBoard& state) override;

private:
    Castling _mv;
};
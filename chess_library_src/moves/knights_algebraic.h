#pragma once 

#include "algebraic_notation.h"
#include "knights_move.h"

namespace internal {

class KnightsAlgebraic final : public AlgebraicNotation {
public:
    
    KnightsAlgebraic(KnightsMove mv, std::string resolveAmbiguity) noexcept;

    MoveResult performMove(const ChessBoard& state) override;

private:
    KnightsMove _mv;
    std::string _resolveAmbiguity;
};

}
#pragma once

#include "algebraic_notation.h"
#include "underpromotion.h"

namespace internal {

class UnderpromotionAlgebraic final : AlgebraicNotation {
public:
    
    UnderpromotionAlgebraic(Underpromotion mv, std::string resolveAmbiguity) noexcept;

    MoveResult performMove(const ChessBoard& board) override;

private:
    Underpromotion _mv;
    std::string _resolveAmbiguity;
};


}
#pragma once

#include "algebraic_notation.h"
#include "underpromotion.h"

class UnderpromotionAlgebraic final : AlgebraicNotation {
public:
    
    UnderpromotionAlgebraic(Underpromotion mv, std::string resolveAmbiguity) noexcept;

    MoveResult performMove(const ChessBoard& state) override;

private:
    Underpromotion _mv;
    std::string _resolveAmbiguity;
};
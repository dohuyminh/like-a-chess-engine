#pragma once

#include "reachability.h"

class RKingKnight final : public SquareReachability {
public:
    
    RKingKnight(const Vec2D* mvs) noexcept;

    std::vector<Coord2D> pieceCanReachSquare(
        const ChessBoard& state, 
        Coord2D origin) const override;

private:
    const Vec2D* _mvs;
};
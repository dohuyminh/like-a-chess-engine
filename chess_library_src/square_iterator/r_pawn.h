#pragma once

#include "reachability.h"

class RPawn final : public SquareReachability {
public:
    
    RPawn() noexcept;

    std::vector<Coord2D> pieceCanReachSquare(
        const ChessBoard& state, 
        Coord2D origin) const override;

};
#pragma once 

#include "reachability.h"

class RRook final : public SquareReachability {
public:
    
    RRook() noexcept;

    std::vector<Coord2D> pieceCanReachSquare(
        const ChessBoard& state, 
        Coord2D origin) const;

};
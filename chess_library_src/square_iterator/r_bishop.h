#pragma onc

#include "reachability.h"

class RBishop final : public SquareReachability {
public:
    
    RBishop() noexcept = default;

    [[nodiscard]] std::vector<Coord2D> pieceCanReachSquare(
        const ChessBoard& state, 
        Coord2D origin) const override;

};
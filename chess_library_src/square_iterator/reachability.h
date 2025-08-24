#pragma once 

#include "../chess_board.h"

#include <vector>

class SquareReachability {
public:
    virtual ~SquareReachability() = default;

    [[nodiscard]] virtual std::vector<Coord2D> pieceCanReachSquare(
        const ChessBoard& state, 
        Coord2D origin) const = 0;
};
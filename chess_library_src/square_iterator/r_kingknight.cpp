#include "r_kingknight.h"

#include <stdexcept>

RKingKnight::RKingKnight(const Vec2D* mvs) noexcept : _mvs(mvs) {}

std::vector<Coord2D> RKingKnight::pieceCanReachSquare(const ChessBoard& state, Coord2D origin) const {

    std::vector<Coord2D> res;
    
    ChessPiece pieceAtOrigin = state.getPiece(origin);
    if (pieceAtOrigin.isNone()) {
        return res;
    }

    for (int8_t i = 0; i < 8; ++i) {
        try {
            Coord2D dest = origin + _mvs[i];
            ChessPiece pieceAtDest = state.getPiece(dest);

            if (pieceAtDest.isNone() || pieceAtOrigin.captures(pieceAtDest)) {

                res.push_back(dest);
            }            

        } catch (std::invalid_argument const&) {
            continue;
        }
    }

    return res;
}
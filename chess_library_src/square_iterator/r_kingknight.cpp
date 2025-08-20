#include "r_kingknight.h"

#include <stdexcept>

RKingKnight::RKingKnight(const Vec2D* mvs) noexcept : _mvs(mvs) {}

std::vector<Coord2D> RKingKnight::pieceCanReachSquare(const ChessBoard& state, Coord2D origin) const {

    std::vector<Coord2D> res;
    
    Piece_t pieceAtOrigin = state.getPiece(origin);
    if (pieceAtOrigin == static_cast<Piece_t>(ChessPiece::NONE)) {
        return res;
    }

    auto capture = pieceIsWhite(pieceAtOrigin) ? pieceIsBlack : pieceIsWhite;  

    for (int8_t i = 0; i < 8; ++i) {
        try {
            Coord2D dest = origin + _mvs[i];
            Piece_t pieceAtDest = state.getPiece(dest);

            if (pieceAtDest == static_cast<Piece_t>(ChessPiece::NONE) || 
                capture(pieceAtDest)) {

                res.push_back(dest);
            }            

        } catch (std::invalid_argument const&) {
            continue;
        }
    }

    return res;
}
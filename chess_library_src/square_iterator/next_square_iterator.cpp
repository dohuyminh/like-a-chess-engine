#include "next_square_iterator.h"
#include "r_kingknight.h"
#include "r_pawn.h"
#include "r_rook.h"
#include "r_bishop.h"
#include "r_composite.h"

std::vector<Coord2D> findAvailableSquares(const ChessBoard& state, Coord2D pieceCoord) {

    ChessPiece pieceAtOrigin = state.getPiece(pieceCoord);
    
    // if there is no piece at the square -> return empty 
    if (pieceAtOrigin.isNone()) {
        return {};
    }

    // create the reachability object
    RComposite generator;
    
    if (pieceAtOrigin.isKing() || pieceAtOrigin.isKnight()) {
        generator.addComponent(new RKingKnight(pieceAtOrigin.isKing() ? vecMap : knightsMoveVec));
    }

    if (pieceAtOrigin.isPawn()) {
        generator.addComponent(new RPawn());
    }

    if (pieceAtOrigin.isRook() || pieceAtOrigin.isQueen()) {
        generator.addComponent(new RRook());
    }

    if (pieceAtOrigin.isBishop() || pieceAtOrigin.isQueen()) {
        generator.addComponent(new RBishop());
    }

    return generator.pieceCanReachSquare(state, pieceCoord);
}
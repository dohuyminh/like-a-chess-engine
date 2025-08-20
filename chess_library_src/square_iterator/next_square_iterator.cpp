#include "next_square_iterator.h"
#include "r_kingknight.h"
#include "r_pawn.h"
#include "r_rook.h"
#include "r_bishop.h"
#include "r_composite.h"

#include <memory>

std::vector<Coord2D> findAvailableSquares(const ChessBoard& state, Coord2D pieceCoord) {

    Piece_t pieceAtOrigin = state.getPiece(pieceCoord);
    
    // if there is no piece at the square -> return empty 
    if (pieceAtOrigin == static_cast<Piece_t>(ChessPiece::NONE)) {
        return {};
    }

    // create the reachability object
    RComposite generator;
    
    bool isKing = pieceAtOrigin == static_cast<Piece_t>(ChessPiece::WHITE_KING) || 
                  pieceAtOrigin == static_cast<Piece_t>(ChessPiece::BLACK_KING);

    bool isKnight = pieceAtOrigin == static_cast<Piece_t>(ChessPiece::WHITE_KNIGHT) || 
                    pieceAtOrigin == static_cast<Piece_t>(ChessPiece::BLACK_KNIGHT);

    if (isKing || isKnight) {
        generator.addComponent(new RKingKnight(isKing ? vecMap : knightsMoveVec));
    }

    bool isPawn = pieceAtOrigin == static_cast<Piece_t>(ChessPiece::WHITE_PAWN) || 
                  pieceAtOrigin == static_cast<Piece_t>(ChessPiece::BLACK_PAWN);

    if (isPawn) {
        generator.addComponent(new RPawn());
    }

    bool isQueen = pieceAtOrigin == static_cast<Piece_t>(ChessPiece::WHITE_QUEEN) || 
                   pieceAtOrigin == static_cast<Piece_t>(ChessPiece::BLACK_QUEEN);

    bool isRook = pieceAtOrigin == static_cast<Piece_t>(ChessPiece::WHITE_ROOK) || 
                  pieceAtOrigin == static_cast<Piece_t>(ChessPiece::BLACK_ROOK);

    if (isRook || isQueen) {
        generator.addComponent(new RRook());
    }

    bool isBishop = pieceAtOrigin == static_cast<Piece_t>(ChessPiece::WHITE_BISHOP) || 
                    pieceAtOrigin == static_cast<Piece_t>(ChessPiece::BLACK_BISHOP);

    if (isBishop || isQueen) {
        generator.addComponent(new RBishop());
    }

    return generator.pieceCanReachSquare(state, pieceCoord);
}
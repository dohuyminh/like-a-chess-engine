#include "chess_move.h"

ChessMove::ChessMove(const bool appliedPieceIsWhite) : _isWhite(appliedPieceIsWhite) {}

/**
 * This method is used to update the opponent's castling rights, given some move might capture a rook in its initial position.
 *
 * @param state The original state to be transitioned
 * @param originPieceIsWhite Whose turn it is to play (white: true, black: false)
 * @param newPoint The new position of the piece to be moved
 * @param newWhiteLeftCastling Reference to white player's left castling's right
 * @param newWhiteRightCastling Reference to white player's right castling's right
 * @param newBlackLeftCastling Reference to black player's left castling's right
 * @param newBlackRightCastling Reference to black player's right castling's right
 */
void ChessMove::updateCastling(
    const ChessBoard& state, 
    const bool originPieceIsWhite,
    const Coord2D newPoint,
    bool& newWhiteLeftCastling, 
    bool& newWhiteRightCastling, 
    bool& newBlackLeftCastling, 
    bool& newBlackRightCastling) {

    if (originPieceIsWhite) {
        // if the rook is at the original place and castling right still exists
        // capture it and castling right is forfeit
        if (state.blackLeftCastling() && newPoint == Coord2D('h', 8)) {
            newBlackLeftCastling = false; 
        } 
        else if (state.blackRightCastling() && newPoint == Coord2D('a', 8)) {
            newBlackRightCastling = false;
        }
    }   
    else {
        // same logic applies
        if (state.whiteLeftCastling() && newPoint == Coord2D('a', 1)) {
            newWhiteLeftCastling = false; 
        } 
        else if (state.whiteRightCastling() && newPoint == Coord2D('h', 1)) {
            newWhiteRightCastling = false;
        }
    }
}
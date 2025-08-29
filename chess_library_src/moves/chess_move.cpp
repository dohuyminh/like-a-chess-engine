#include "chess_move.h"
#include "../utility.h"

namespace internal
{

/**
 * This method is used to update the turn's castling rights, given some move might capture a rook in its initial position.
 *
 * @param state The original state to be transitioned
 * @param turn Whose turn it is to play
 * @param newPoint The new position of the piece to be moved
 * @param boardData The board data of the next state
 */
void ChessMove::updateCastling(
    const ChessBoard& state, 
    Color turn,
    Coord2D newPoint,
    char* boardData) {

    if (turn == Color::WHITE) {
        // if the rook is at the original place and castling right still exists
        // capture it and castling right is forfeit
        if (state.castling(~turn, true) && newPoint == Coord2D('h', 8)) {
            internal::utility::turnOffCastling(boardData, ~turn, true);
        } 
        else if (state.castling(~turn, false) && newPoint == Coord2D('a', 8)) {
            internal::utility::turnOffCastling(boardData, ~turn, false);
        }
    }   
    else {
        // same logic applies
        if (state.castling(~turn, true) && newPoint == Coord2D('a', 1)) {
            internal::utility::turnOffCastling(boardData, ~turn, true);
        } 
        else if (state.castling(~turn, false) && newPoint == Coord2D('h', 1)) {
            internal::utility::turnOffCastling(boardData, ~turn, false);
        }
    }
}
    
} // namespace internal

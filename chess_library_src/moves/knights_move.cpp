#include "knights_move.h"
#include "../utility.h"

#include <stdexcept>

namespace internal {

/**
 * @brief Constructor for the Knight's move.
 * 
 * @param color the color of the knight
 * @param direction The direction at which the piece will move (NOTE: the direction can only either be <1,2>, <1,-2>, <2,1>, <2,-1>, <-1,2>, <-1,-2>, <-2,1> or <-2,-1>, reflecting the knight's "L" shape movement)
 * @param origin The location of the knight piece on the board that will be moved
 */
KnightsMove::KnightsMove(Color color, Vec2D direction, Coord2D origin) :
    ChessMove(color),
    _direction(direction),
    _origin(origin) {

    const int8_t dx = abs(_direction.mvCol()), dy = abs(_direction.mvRow());

    // validate knight's movement
    if (dx + dy != 3 || dx < 1 || dx > 2 || dy < 1 || dy > 2) {
        throw std::invalid_argument("Knight's movement may only be <1,2>, <1,-2>, <2,1>, <2,-1>, <-1,2>, <-1,-2>, <-2,1>, <-2,-1>");
    }

    // if the knight is black -> reverse direction on board 
    if (_color == Color::BLACK) {
        _direction *= -1;
    }

    // if the movement results in out-of-bound coordinate -> throw a tantrum lol
    try {
        _origin + _direction;
    } catch(std::invalid_argument const&) {
        throw std::invalid_argument("Knight's movement results in out-of-bound coordinate");
    }
}

/**
 * @brief This function applies the Knight's move to the given chess board board.
 * 
 * @param board The current board to be transitioned 
 * @return A new board if the transition is valid, or std::nullopt otherwise
 */
std::optional<ChessBoard> KnightsMove::operator()(const ChessBoard& board) const {
    // locate the position of piece on board 
    ChessPiece piece = board.getPiece(_origin);

    // if there is no piece at the square, simply return invalid
    if (piece.isNone()) {
        return std::nullopt;
    }
    
    // if the piece is not a knight, return invalid
    if (!piece.isKnight()) {
        return std::nullopt;
    }   
    
    // does transformation apply to the correct piece color?
    if (_color != piece.color()) {
        return std::nullopt;
    }
            
    // get new point
    Coord2D dest = _origin + _direction;

    // check if the position is valid (i.e. knights cannot capture pieces of the same color)
    if (piece.color() == board.getPiece(dest).color()) {

        return std::nullopt;
    }

    // get the raw board
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);

    // transform the raw board 
    internal::utility::writeData(boardData, _origin, ChessPiece::NONE);
    internal::utility::writeData(boardData, dest, piece);
    
    // update castling + en passant
    updateCastling(board, _color, dest, boardData);
    internal::utility::turnOffEnpassant(boardData);

    // return final board 
    return ChessBoard(boardData); 
}

}
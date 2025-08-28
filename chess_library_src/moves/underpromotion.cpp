#include "underpromotion.h"
#include "../utility.h"

#include <stdexcept>
#include <cstring>

/**
 * @brief Construct for the Underpromotion's move 
 * 
 * @param color The color of the pawn
 * @param direction The direction at which the piece will move (NOTE: the direction can only either be UP, UP_LEFT or UP_RIGHT)
 * @param origin The location of the pawn piece on the board that will be moved
 * @param promotePiece The piece that the pawn will be promoted to (NOTE: the piece can only be a rook, bishop or knight of the same color)
 */
Underpromotion::Underpromotion(Color color, Direction direction, Coord2D origin, ChessPiece promotePiece) :
    ChessMove(color),
    _direction(direction), 
    _origin(origin),
    _promotePiece(promotePiece)
{

    // promotion should only be done at the 2nd first/last row in the board 
    if ((_color == Color::BLACK && _origin.row() != 2) || (_color == Color::WHITE && _origin.row() != 7)) {
        throw std::invalid_argument("Promotion may only apply for pawns at the 2nd last rows of their respective colors"); 
    }

    // verify if the direction is only up/up left/up right 
    if (_direction != Direction::UP && _direction != Direction::UP_LEFT && _direction != Direction::UP_RIGHT) {
        throw std::invalid_argument("Pawn may only move in the direction of Up/Up-Left/Up-Right");
    } 

    // check if pieces at the edge are trying to go out of bound 
    if (
        (_color == Color::WHITE && (
            (_origin == Coord2D('a', 7) && _direction == Direction::UP_LEFT) || 
            (_origin == Coord2D('h', 7) && _direction == Direction::UP_RIGHT)
        )) || 
        (_color == Color::BLACK && (
            (_origin == Coord2D('a', 2) && _direction == Direction::UP_RIGHT) ||
            (_origin == Coord2D('h', 2) && _direction == Direction::UP_LEFT)
        ))
    ) {
        throw std::invalid_argument("Pawn's move results in out-of-bound coordinate");
    }

    // promoted pawn can only be rook/bishop/knight
    if ((_color == Color::BLACK && 
        _promotePiece != ChessPiece::BLACK_ROOK &&
        _promotePiece != ChessPiece::BLACK_KNIGHT && 
        _promotePiece != ChessPiece::BLACK_BISHOP) ||
        (_color == Color::WHITE &&
        _promotePiece != ChessPiece::WHITE_BISHOP && 
        _promotePiece != ChessPiece::WHITE_KNIGHT && 
        _promotePiece != ChessPiece::WHITE_ROOK)) {

        throw std::invalid_argument("Underpromoted pawn may only be promoted to Rook/Bishop/Knight of the same color");
    }
}

/**
 * @brief This function applies the Underpromotion's move to the given chess board state.
 * 
 * @param state The current state to be transitioned
 * @return A new state if the transition is valid, or std::nullopt otherwise
 */
std::optional<ChessBoard> Underpromotion::operator()(const ChessBoard& state) const {
    // get the piece at origin 
    ChessPiece piece = state.getPiece(_origin);
    if (!piece.isPawn()) {
        return std::nullopt;
    }

    // check if the transformation is applied to the right piece 
    if (_color != piece.color()) {
        return std::nullopt;
    }

    // get raw board 
    char boardData[34] = { 0 };
    std::copy(state.boardData(), state.boardData() + 34, boardData);

    // get direction and new piece position after transformation
    Vec2D dir = vecMap[_direction];
    if (_color == Color::BLACK) dir *= -1;

    Coord2D dest = _origin + dir;

    ChessPiece pieceAtDest = state.getPiece(dest);

    // check if the destination is valid (i.e. pawn cannot capture piece of the same color)
    if (pieceAtDest.color() == _color) {

        return std::nullopt;
    }

    // diagonal moves may only be used for capturing pieces
    if ((_direction == Direction::UP_LEFT || _direction == Direction::UP_RIGHT) && pieceAtDest.isNone()) {
        return std::nullopt;
    }

    // forward move is impossible if there is a piece in front
    if (_direction == Direction::UP && !pieceAtDest.isNone()) {
        return std::nullopt;
    }

    // update castling + en passant 
    updateCastling(state, _color, dest, boardData);
    internal::utility::turnOffEnpassant(boardData);

    uint8_t originIdx = _origin.toFlatIdx(), destIdx = dest.toFlatIdx();
    // perform transformation on raw board 
    internal::utility::writeData(boardData, _origin, ChessPiece::NONE);
    internal::utility::writeData(boardData, dest, _promotePiece);

    return ChessBoard(boardData);
}
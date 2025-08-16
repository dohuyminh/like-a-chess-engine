#include "underpromotion.h"

#include <stdexcept>

/**
 * @brief Construct for the Underpromotion's move 
 * 
 * @param appliedPieceIsWhite whether it is white player's turn to play (white: true, black: false)
 * @param direction The direction at which the piece will move (NOTE: the direction can only either be UP, UP_LEFT or UP_RIGHT)
 * @param origin The location of the pawn piece on the board that will be moved
 * @param promotePiece The piece that the pawn will be promoted to (NOTE: the piece can only be a rook, bishop or knight of the same color)
 */
Underpromotion::Underpromotion(const bool appliedPieceIsWhite, const Direction direction, const Coord2D origin, const ChessPiece promotePiece) :
    ChessMove(appliedPieceIsWhite),
    _direction(direction), 
    _origin(origin),
    _promotePiece(promotePiece)
{

    // promotion should only be done at the 2nd first/last row in the board 
    if ((!_isWhite && _origin.row() != 2) || (_isWhite && _origin.row() != 7)) {
        throw std::invalid_argument("Promotion may only apply for pawns at the 2nd last rows of their respective colors"); 
    }

    // verify if the direction is only up/up left/up right 
    if (_direction != Direction::UP && _direction != Direction::UP_LEFT && _direction != Direction::UP_RIGHT) {
        throw std::invalid_argument("Pawn may only move in the direction of Up/Up-Left/Up-Right");
    } 

    // check if pieces at the edge are trying to go out of bound 
    if (
        (_isWhite && (
            (_origin == Coord2D('a', 7) && _direction == Direction::UP_LEFT) || 
            (_origin == Coord2D('h', 7) && _direction == Direction::UP_RIGHT)
        )) || 
        (!_isWhite && (
            (_origin == Coord2D('a', 2) && _direction == Direction::UP_RIGHT) ||
            (_origin == Coord2D('h', 2) && _direction == Direction::UP_LEFT)
        ))
    ) {
        throw std::invalid_argument("Pawn's move results in out-of-bound coordinate");
    }

    // promoted pawn can only be rook/bishop/knight
    if ((!_isWhite && 
        _promotePiece != ChessPiece::BLACK_ROOK &&
        _promotePiece != ChessPiece::BLACK_KNIGHT && 
        _promotePiece != ChessPiece::BLACK_BISHOP) ||
        (_isWhite &&
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
    Piece_t piece = state.getPiece(_origin);

    // check the color of the piece 
    bool pawnIsWhite = true;
    
    // if the pawn is white -> switch
    if (piece == static_cast<char>(ChessPiece::BLACK_PAWN)) {
        pawnIsWhite = false;
    }

    // neither black/white pawn -> transformation on an invalid piece 
    else if (piece != static_cast<char>(ChessPiece::WHITE_PAWN)) {
        return std::nullopt;
    }
        
    // check if the transformation is applied to the right piece 
    if (pawnIsWhite != _isWhite) {
        return std::nullopt;
    }

    // get raw board 
    std::string rawBoard = state.board();

    // get direction and new piece position after transformation
    Vec2D dir = vecMap[_direction];
    if (!_isWhite) dir *= -1;

    Coord2D dest = _origin + dir;
    uint8_t originIdx = _origin.toFlatIdx(), destIdx = dest.toFlatIdx();

    // check if the destination is valid (i.e. pawn cannot capture piece of the same color)
    if ((_isWhite && pieceIsWhite(rawBoard[destIdx])) || 
        (!_isWhite && pieceIsBlack(rawBoard[destIdx]))) {

        return std::nullopt;
    }

    // diagonal moves may only be used for capturing pieces
    if ((_direction == Direction::UP_LEFT || _direction == Direction::UP_RIGHT) && rawBoard[destIdx] == static_cast<char>(ChessPiece::NONE)) {
        return std::nullopt;
    }

    // forward move is impossible if there is a piece in front
    if (_direction == Direction::UP && rawBoard[destIdx] != static_cast<char>(ChessPiece::NONE)) {
        return std::nullopt;
    }

    // update en passant square (since a pawn is moving at the 2nd last row, there shall be no en passant square left)
    constexpr std::optional<Coord2D> whiteEnpassant = std::nullopt;
    constexpr std::optional<Coord2D> blackEnpassant = std::nullopt;

    // update castling square 
    bool newWhiteLeftCastling = state.whiteLeftCastling(), 
        newWhiteRightCastling = state.whiteRightCastling(), 
        newBlackLeftCastling  = state.blackLeftCastling(),
        newBlackRightCastling = state.blackRightCastling();

    updateCastling(state, pawnIsWhite, dest, newWhiteLeftCastling, newWhiteRightCastling, newBlackLeftCastling, newBlackRightCastling);

    // perform transformation on raw board 
    rawBoard[originIdx] = static_cast<char>(ChessPiece::NONE);
    rawBoard[destIdx] = static_cast<char>(_promotePiece);

    return ChessBoard(
        rawBoard, 
        state.whiteKingCoord(),
        state.blackKingCoord(),
        newWhiteLeftCastling, 
        newWhiteRightCastling, 
        newBlackLeftCastling, 
        newBlackRightCastling, 
        whiteEnpassant, 
        blackEnpassant);
}
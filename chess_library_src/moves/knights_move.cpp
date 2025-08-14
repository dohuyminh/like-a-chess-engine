#include "knights_move.h"

#include <stdexcept>

/**
 * @brief Constructor for the Knight's move.
 * 
 * @param appliedPieceIsWhite whether it is white player's turn to play (white: true, black: false) 
 * @param direction The direction at which the piece will move (NOTE: the direction can only either be <1,2>, <1,-2>, <2,1>, <2,-1>, <-1,2>, <-1,-2>, <-2,1> or <-2,-1>, reflecting the knight's "L" shape movement)
 * @param origin The location of the knight piece on the board that will be moved
 */
KnightsMove::KnightsMove(const bool appliedPieceIsWhite, const Vec2D direction, const Coord2D origin) :
    ChessMove(appliedPieceIsWhite),
    _direction(direction),
    _origin(origin) {

    const int8_t dx = abs(_direction.mvCol()), dy = abs(_direction.mvRow());

    // validate knight's movement
    if (dx + dy != 3 || dx < 1 || dx > 2 || dy < 1 || dy > 2) {
        throw std::invalid_argument("Knight's movement may only be <1,2>, <1,-2>, <2,1>, <2,-1>, <-1,2>, <-1,-2>, <-2,1>, <-2,-1>");
    }

    // if the knight is black -> reverse direction on board 
    if (!_isWhite) {
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
 * @brief This function applies the Knight's move to the given chess board state.
 * 
 * @param state The current state to be transitioned 
 * @return A new state if the transition is valid, or std::nullopt otherwise
 */
std::optional<ChessBoard> KnightsMove::operator()(const ChessBoard& state) const {
    // locate the position of piece on board 
    const Piece_t piece = state.getPiece(_origin);

    // if there is no piece at the square, simply return invalid
    if (piece == static_cast<char>(ChessPiece::NONE)) {
        return std::nullopt;
    }
    
    // if the piece is not a knight, return invalid
    if (piece != static_cast<char>(ChessPiece::WHITE_KNIGHT) && 
        piece != static_cast<char>(ChessPiece::BLACK_KNIGHT)) {
        return std::nullopt;
    }   

    // knight is white/black
    const bool knightIsWhite = piece == static_cast<char>(ChessPiece::WHITE_KNIGHT);

    // does transformation apply to the correct piece color?
    if (knightIsWhite != _isWhite) {
        return std::nullopt;
    }

    // get the raw board
    std::string rawBoard = state.board();
        
    // get new point
    Coord2D newPoint = _origin + _direction;

    const uint8_t originIdx = _origin.toFlatIdx();
    // move
    const uint8_t destIdx = newPoint.toFlatIdx();

    // check if the position is valid (i.e. knights cannot capture pieces of the same color)
    if ((knightIsWhite && pieceIsWhite(rawBoard[destIdx])) || 
        (!knightIsWhite && pieceIsBlack(rawBoard[destIdx]))) {

        return std::nullopt;
    }
        
    // update enpassant squares (since the knight has moved, the opponent's enpassant square is invalidated)
    // since in this turn, a knight moves, there will not be any enpassant square for the home player
    std::optional<Coord2D> newWhiteEnpassant = std::nullopt;
    std::optional<Coord2D> newBlackEnpassant = std::nullopt; 
    
    // update castling rights
    bool newWhiteLeftCastling = state.whiteLeftCastling(),
        newWhiteRightCastling = state.whiteRightCastling(), 
        newBlackLeftCastling  = state.blackLeftCastling(), 
        newBlackRightCastling = state.blackRightCastling();
    
    // update castling
    updateCastling(state, knightIsWhite, newPoint, newWhiteLeftCastling, newWhiteRightCastling, newBlackLeftCastling, newBlackRightCastling);

    // transform the raw board 
    rawBoard[originIdx] = static_cast<char>(ChessPiece::NONE);
    rawBoard[destIdx] = piece;

    // return final state 
    return ChessBoard(
        rawBoard, 
        state.whiteKingCoord(),
        state.blackKingCoord(),
        newWhiteLeftCastling, 
        newWhiteRightCastling, 
        newBlackLeftCastling, 
        newBlackRightCastling, 
        newWhiteEnpassant, 
        newBlackEnpassant
    ); 
}
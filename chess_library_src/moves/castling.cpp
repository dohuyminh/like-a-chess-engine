#include "castling.h"
#include "../check_terminal/check.h"

/**
 * @brief Construct for the Castling's move
 * 
 * @param isWhiteTurn whether it is white player's turn to play (white: true, black: false)
 * @param isLeft whether the castling is done with the left rook (true) or the right rook (false)
 */
Castling::Castling(Color color, bool isLeft) noexcept : 
    ChessMove(color), 
    _isLeft(isLeft) {}

/**
 * @brief This function applies the Castling's move to the given chess board state.
 * 
 * @param state The current state to be transitioned
 * @return A new state if the transition is valid, or std::nullopt otherwise
 */
std::optional<ChessBoard> Castling::operator()(const ChessBoard& state) const {
    // check if castling is allowed in the first place 
    if (_color == Color::WHITE) {
        if ((_isLeft && !state.whiteLeftCastling()) || (!_isLeft && !state.whiteRightCastling())) {
            return std::nullopt;
        }
    }
    if ((_isLeft && !state.blackLeftCastling()) || (!_isLeft && !state.blackRightCastling())) {
        return std::nullopt;
    }

    // if the king is in check, castling is impossible
    if (!CheckTerminal::kingIsChecked(state, _color).empty()) {
        return std::nullopt;
    }

    // navigating appropriate rook and how they should move depends on _isWhite and _isLeft
    bool navigate = (_color == Color::WHITE && _isLeft) || (_color == Color::BLACK  && !_isLeft);

    // check which king and which rook is going to move
    int8_t row = (_color == Color::WHITE) ? 1 : 8;
    char rookCol = navigate ? 'a' : 'h';
    Vec2D mv = navigate ? Vec2D(1, 0) : Vec2D(-1, 0);
    
    Coord2D rookPos = Coord2D(rookCol, row);
    Coord2D kingPos = Coord2D('e', row);

    // get raw board 
    std::string rawBoard = state.board();

    // check if there are any pieces between the king and the rook
    for (Coord2D iter = rookPos + mv; iter != kingPos; iter = iter + mv) {
        // there exists a piece in between -> castling is impossible
        if (rawBoard[iter.toFlatIdx()] != ChessPiece::NONE) {
            return std::nullopt;
        }
    }

    // update en passant square
    std::optional<Coord2D> whiteEnpassant = std::nullopt, blackEnpassant = std::nullopt;

    // update castling rights
    bool newWhiteLeftCastling = state.whiteLeftCastling(), 
    newWhiteRightCastling = state.whiteRightCastling(), 
    newBlackLeftCastling  = state.blackLeftCastling(), 
    newBlackRightCastling = state.blackRightCastling();
    
    if (_color == Color::WHITE) {
        if (_isLeft) newWhiteLeftCastling = false;
        else newWhiteRightCastling = false;
    }
    else {
        if (_isLeft) newBlackLeftCastling = false;
        else newBlackRightCastling = false;
    }
    
    // perform transformation
    Piece_t king = rawBoard[kingPos.toFlatIdx()], rook = rawBoard[rookPos.toFlatIdx()];
    
    Vec2D kingMv = mv * -2;
    Coord2D newKingPos = kingPos + kingMv;

    rawBoard[kingPos.toFlatIdx()] = ChessPiece::NONE;
    rawBoard[newKingPos.toFlatIdx()] = king;

    rawBoard[rookPos.toFlatIdx()] = ChessPiece::NONE;
    rawBoard[(newKingPos + mv).toFlatIdx()] = rook;

    Coord2D newWhiteKingCoord = state.whiteKingCoord(), newBlackKingCoord = state.blackKingCoord();

    if (_color == Color::WHITE) {
        newWhiteKingCoord = newKingPos;
    } else {
        newBlackKingCoord = newKingPos;
    }

    return ChessBoard(
        rawBoard, 
        newWhiteKingCoord,
        newBlackKingCoord,
        newWhiteLeftCastling, 
        newWhiteRightCastling, 
        newBlackLeftCastling, 
        newBlackRightCastling, 
        whiteEnpassant, 
        blackEnpassant
    );
}

#include "castling.h"
#include "../check_terminal/check.h"
#include "../utility.h"

#include <cstring>
#include <optional>

/**
 * @brief Construct for the Castling's move
 * 
 * @param color the color of the player performing castling
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
    if (!state.castling(_color, _isLeft)) {
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

    // check if there are any pieces between the king and the rook
    for (Coord2D iter = rookPos + mv; iter != kingPos; iter = iter + mv) {
        // there exists a piece in between -> castling is impossible
        if (!state.getPiece(iter).isNone()) {
            return std::nullopt;
        }
    }

    // get raw board 
    char boardData[34] = { 0 };
    std::copy(state.boardData(), state.boardData() + 34, boardData);
    
    // disable castling for the move 
    internal::utility::turnOffCastling(boardData, _color, true);
    internal::utility::turnOffCastling(boardData, _color, false);

    // perform transformation
    Vec2D kingMv = mv * -2;
    Coord2D newKingPos = kingPos + kingMv;

    internal::utility::writeData(boardData, kingPos, ChessPiece::NONE);
    internal::utility::writeData(boardData, newKingPos, state.getPiece(kingPos));

    internal::utility::writeData(boardData, rookPos, ChessPiece::NONE);
    internal::utility::writeData(boardData, newKingPos + mv, state.getPiece(rookPos));

    // disable en passant 
    internal::utility::turnOffEnpassant(boardData);

    return ChessBoard(boardData);
}

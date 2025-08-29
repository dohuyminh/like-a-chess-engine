#include "queens_algebraic.h"
#include "../check_terminal/checkmate.h"

#include <stdexcept>

namespace internal {

QueensAlgebraic::QueensAlgebraic(QueensMove mv, std::string resolveAmbiguity) noexcept :
    _mv(mv), _resolveAmbiguity(resolveAmbiguity) {}

MoveResult QueensAlgebraic::performMove(const ChessBoard& state) {

    using namespace CheckTerminal;

    // ensure the given move is valid 
    std::optional<ChessBoard> nextState = _mv(state);
    if (!nextState.has_value()) {
        throw std::invalid_argument("The queen's move is not valid for the given state");
    }
    
    std::string an;
    ChessPiece pieceAtOrigin = state.getPiece(_mv.origin());
    // depends on which piece is being moved, note it at the beginning of the move's name
    
    if (pieceAtOrigin.isKing())        an.push_back('K');
    else if (pieceAtOrigin.isQueen())  an.push_back('Q');
    else if (pieceAtOrigin.isRook())   an.push_back('R');
    else if (pieceAtOrigin.isBishop()) an.push_back('B');
    
    // resolve any ambiguity
    an += _resolveAmbiguity;
    
    // confirm if the piece captured something
    Coord2D dest = _mv.origin() + _mv.moveVec();
    if (!state.getPiece(dest).isNone() || (pieceAtOrigin.isPawn() && state.enpassant(~pieceAtOrigin.color()) == dest)) {
        an.push_back('x');
    }

    // insert destination square 
    an.push_back(dest.col());
    an.push_back(dest.row() + '0');

    // if it's a pawn reaching the last row, promote it to Queen
    if (pieceAtOrigin.isPawn() && (pieceAtOrigin.isWhite() && dest.row() == 8) || (pieceAtOrigin.isBlack() && dest.row() == 1)) {
        an.push_back('=');
        an.push_back('Q');
    }

    // if the next state results in opponent's king being checked, note that
    MateStatus ms = isCheckmate(nextState.value(), ~_mv.colorOfAppliedPiece());
    if (ms == MateStatus::CHECK) {
        an.push_back('x');
    } else if (ms == MateStatus::CHECKMATE) {
        an.push_back('x');
        an.push_back('x');
    }

    return { an, nextState.value() };
}

}
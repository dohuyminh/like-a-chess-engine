#include "queens_algebraic.h"
#include "../check_terminal/checkmate.h"

#include <stdexcept>

namespace internal {

QueensAlgebraic::QueensAlgebraic(QueensMove mv) noexcept :
    _mv(mv)
{}

MoveResult QueensAlgebraic::performMove(const ChessBoard& board) {

    using namespace CheckTerminal;

    bool capture = false;
    bool pawnMoved = false;

    // ensure the given move is valid 
    std::optional<ChessBoard> nextboard = _mv(board);
    if (!nextboard.has_value()) {
        throw std::invalid_argument("The queen's move is not valid for the given board");
    }
    
    std::string an;
    ChessPiece pieceAtOrigin = board.getPiece(_mv.origin());
    // depends on which piece is being moved, note it at the beginning of the move's name
    
    if (pieceAtOrigin.isKing())        an.push_back('K');
    else if (pieceAtOrigin.isQueen())  an.push_back('Q');
    else if (pieceAtOrigin.isRook())   an.push_back('R');
    else if (pieceAtOrigin.isBishop()) an.push_back('B');
    else                               pawnMoved = true; // if it's a pawn, we don't note anything but we do note that a pawn has moved

    // resolve any ambiguity
    an += _mv.origin();
    
    // confirm if the piece captured something
    Coord2D dest = _mv.origin() + _mv.moveVec();
    if (!board.getPiece(dest).isNone() || (pieceAtOrigin.isPawn() && board.enpassant(~pieceAtOrigin.color()) == dest)) {
        capture = true;
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

    // if the next board results in opponent's king being checked, note that
    MateStatus ms = isCheckmate(nextboard.value(), ~_mv.colorOfAppliedPiece());
    if (ms == MateStatus::CHECK) {
        an.push_back('+');
    } else if (ms == MateStatus::CHECKMATE) {
        an.push_back('#');
    }

    return { an, nextboard.value(), capture, pawnMoved };
}

}
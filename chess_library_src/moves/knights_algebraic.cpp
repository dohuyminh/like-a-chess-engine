#include "knights_algebraic.h"
#include "../check_terminal/checkmate.h"

#include <stdexcept>

namespace internal {

KnightsAlgebraic::KnightsAlgebraic(KnightsMove mv, std::string resolveAmbiguity) noexcept :
    _mv(mv), _resolveAmbiguity(resolveAmbiguity) {}

MoveResult KnightsAlgebraic::performMove(const ChessBoard& board) {

    using namespace CheckTerminal;

    // ensure the move is valid given the board 
    std::optional<ChessBoard> nextboard = _mv(board);
    if (!nextboard.has_value()) {
        throw std::invalid_argument("The knight's move is not valid for the given board");
    }

    std::string an = "N";
    
    // resolve ambiguity
    an += _resolveAmbiguity;

    // determine whether the knight captures something
    Coord2D dest = _mv.origin() + _mv.moveVec();
    if (!board.getPiece(dest).isNone()) {
        an.push_back('x');
    }

    // insert destination square 
    an.push_back(dest.col());
    an.push_back(dest.row() + '0');

    // if the move results in checking the opponent's king, note that
    MateStatus ms = isCheckmate(nextboard.value(), ~_mv.colorOfAppliedPiece());
    if (ms == MateStatus::CHECK) {
        an.push_back('x');
    } else if (ms == MateStatus::CHECKMATE) {
        an.push_back('x');
        an.push_back('x');
    }

    return { an, nextboard.value() };
}

}
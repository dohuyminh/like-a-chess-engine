#include "castling_algebraic.h"
#include "../check_terminal/checkmate.h"

#include <stdexcept>

namespace internal {

CastlingAlgebraic::CastlingAlgebraic(Castling mv) noexcept : _mv(mv) {}

MoveResult CastlingAlgebraic::performMove(const ChessBoard& state) {
    
    using namespace CheckTerminal;

    // get the next state after performing the move 
    std::optional<ChessBoard> nextState = _mv(state);

    // if the move is invalid -> throw a tantrum
    if (!nextState.has_value()) {
        throw std::invalid_argument("The castling move is not valid for the given state");        
    }

    std::string an = (_mv.moveLeft()) ? "0-0-0" : "0-0"; 
    
    // if the move checks the opponent's king, denote at the end with "x" (check) or "xx" (checkmate)
    MateStatus ms = isCheckmate(nextState.value(), ~_mv.colorOfAppliedPiece());

    if (ms == MateStatus::CHECK) {
        an.push_back('x');
    } else if (ms == MateStatus::CHECKMATE) {
        an.push_back('x');
        an.push_back('x');
    }

    // return final result 
    return { an, nextState.value() };
}

}

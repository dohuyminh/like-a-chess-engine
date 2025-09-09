#include "next_board.h"
#include "queens_algebraic.h"
#include "knights_algebraic.h"
#include "underpromotion_algebraic.h"
#include "castling_algebraic.h"

namespace internal {

MoveResult nextBoard(const ChessBoard& board, const std::shared_ptr< internal::ChessMove >& mv) {
    
    if (!mv) {
        throw std::invalid_argument("Move cannot be null");
    }

    const internal::ChessMove* transform = mv.get();
    
    if (dynamic_cast< const internal::QueensMove* >(transform)) {
        const auto* casted = dynamic_cast< const internal::QueensMove* >(transform);
        return internal::QueensAlgebraic(*casted).performMove(board);
    }
    else if (dynamic_cast< const internal::KnightsMove* >(transform)) {
        const auto* casted = dynamic_cast< const internal::KnightsMove* >(transform);
        return internal::KnightsAlgebraic(*casted).performMove(board);
    }
    else if (dynamic_cast< const internal::Underpromotion* >(transform)) {
        const auto* casted = dynamic_cast< const internal::Underpromotion* >(transform);
        return internal::UnderpromotionAlgebraic(*casted).performMove(board);
    }
    else if (dynamic_cast< const internal::Castling* >(transform)) {
        const auto* casted = dynamic_cast< const internal::Castling* >(transform);
        return internal::CastlingAlgebraic(*casted).performMove(board);
    }
    else {
        throw std::invalid_argument("Unsupported move type");
    }
}

}
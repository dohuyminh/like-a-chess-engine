#include "arena.h"
#include "moves/next_board.h"
#include "check_terminal/checkmate.h"

#include <stdexcept>

using namespace internal;

Arena::Arena(std::size_t historySize) : 
    _historySize(historySize)
{
    if (historySize == 0) {
        throw std::invalid_argument("History size must be at least 1");
    }

    // initialize board state
    _stateHistory.emplace_back(
        ChessBoard(), 
        false, 
        Color::WHITE, 
        Color::NONE, 
        0
    );

    // initialize board count
    _boardCount[_stateHistory.back().board] = 1;
}

void Arena::performMove(const std::shared_ptr< internal::ChessMove >& mv) {
    
    if (!mv) {
        throw std::invalid_argument("Move cannot be null");
    }

    // get next board
    MoveResult result = internal::nextBoard(_stateHistory.back().board, mv);

    // update board count
    _boardCount[result.nextBoard]++;

    // update move logs
    _logs.push_back(result.notation);

    // update state history
    Color nextTurn = ~_stateHistory.back().turn;
    std::size_t nextMovesWithoutProgress = result.capture || result.pawnMoved ? 0 : _stateHistory.back().movesWithoutProgress + 1;
    std::vector< std::shared_ptr< internal::ChessMove > > nextPossibleMoves = internal::getAllMoves(result.nextBoard, nextTurn);

    // check if the next state is terminal
    CheckTerminal::MateStatus ms = CheckTerminal::isCheckmate(result.nextBoard, nextTurn, result);


}


#include "arena.h"
#include "moves/next_state.h"

#include <stdexcept>

Arena::Arena() :
    _currBoard(),
    _movesWithoutProgress(0),
    _boardCount({ {_currBoard, 1} }),
    _turn(Color::WHITE),
    _winner(Color::NONE),
    _logs()
{
    
}

void Arena::performMove(const std::shared_ptr< internal::ChessMove >& mv) {
    
    MoveResult result = internal::nextState(_currBoard, mv);

    // update board state/logs
    _currBoard = result.nextBoard;
    _logs.push_back(result.notation);

    // update board count
    ++_boardCount[_currBoard];

    // update moves without progress
    if (result.capture || result.pawnMoved) {
        _movesWithoutProgress = 0;
    } else {
        ++_movesWithoutProgress;
    }

    // if move results in a checkmate 

    // if move results in a stalemate

    // otherwise, continue the game
}


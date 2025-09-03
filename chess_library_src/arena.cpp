#include "arena.h"

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

void Arena::performMove(const internal::ChessMove& mv) {
    // get new board
    _currBoard = mv(_currBoard).value();

    // switch turn
    _turn = ~_turn;

    // update board occurrence
    ++_boardCount[_currBoard];

    // update moves without progress
}


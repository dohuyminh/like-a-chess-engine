#include "arena.h"

#include <stdexcept>

Arena::Arena(bool setTime) : _setTime(setTime) : _id(-1) {
    
}

void Arena::performMove(const internal::ChessMove& mv) {
    _currBoard = mv(_currBoard);
}
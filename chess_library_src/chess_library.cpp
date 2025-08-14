#include "chess_library.h"

namespace ChessLibrary {

    Match::Match() : 
        _currState(),
        _turn(Color::WHITE), 
        _winner(Color::UNDEFINED),
        _movesWithoutProgress(0), 
        _stateCount() {}

    Match& Match::request() {
        static Match match;
        return match;
    }

    void Match::reset() {
        _currState = ChessBoard();
        _turn = Color::WHITE;
        _winner = Color::UNDEFINED;
        _movesWithoutProgress = 0;
        _stateCount.clear();
    }

}
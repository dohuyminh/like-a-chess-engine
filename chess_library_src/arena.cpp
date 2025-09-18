#include "arena.h"
#include "fen.h"
#include "moves/next_board.h"

#include <stdexcept>
#include <cctype>

using namespace internal;

Arena::Arena(std::size_t historySize) : 
    _historySize(historySize)
{
    if (historySize == 0) {
        throw std::invalid_argument("History size must be at least 1");
    }

    // initialize board state
    _stateHistory.push_back(State(ChessBoard(), Color::WHITE, 0, _generator));

    _boardCount[_stateHistory.back().board] = 1;
}

Arena::Arena(std::string fen, std::size_t historySize) :
    _historySize(historySize) 
{
    if (historySize == 0) {
        throw std::invalid_argument("History size must be at least 1");
    }

    // initialize board state 
    _stateHistory.push_back(internal::loadStateFromFen(fen, _generator));
    
    // init board count
    _boardCount[_stateHistory.back().board] = 1;
}

bool Arena::performMove(const std::shared_ptr< internal::ChessMove >& mv) {
    
    if (!mv) {
        throw std::invalid_argument("Move cannot be null");
    }

    // if the game has already ended, disregard
    if (_stateHistory.back().isTerminal) {
        return false;
    }
        
    // get next board
    MoveResult result = internal::nextBoard(_stateHistory.back().board, mv);

    // update board count and moves without progress
    _boardCount[result.nextBoard]++;
    std::size_t mwp = (result.pawnMoved || result.capture) ? 0 : _stateHistory.back().movesWithoutProgress + 1;

    // update move logs
    _logs.push_back(result.notation);

    Color nextTurn = ~_stateHistory.back().turn;

    // add new state to history
    _stateHistory.push_back(State(result.nextBoard, nextTurn, mwp, _generator));

    // by FIDE, 5-fold repetition would mean automatic draw
    if (_boardCount[result.nextBoard] >= 5) {
        _stateHistory.back().isTerminal = true;
        _stateHistory.back().winner     = Color::NONE;
    }

    // if the history exceeds capacity, delete least recent 
    if (_stateHistory.size() > _historySize) {
        _stateHistory.pop_front();
    }

    return true;
}


#include "arena.h"
#include "moves/next_board.h"
#include "check_terminal/checkmate.h"

#include <stdexcept>

using namespace internal;

State::State(
    const ChessBoard& board, 
    bool isTerminal, 
    Color turn, 
    Color winner, 
    std::size_t movesWithoutProgress
) : board(board), isTerminal(isTerminal), turn(turn), winner(winner), movesWithoutProgress(movesWithoutProgress) {}

Arena::Arena(std::size_t historySize) : 
    _historySize(historySize)
{
    if (historySize == 0) {
        throw std::invalid_argument("History size must be at least 1");
    }

    // initialize board state
    _stateHistory.push_back({
        ChessBoard(), 
        false, 
        Color::WHITE, 
        Color::NONE, 
        0
    });

    // initialize board count
    _boardCount[_stateHistory.back().board] = 1;

    // initialize cache
    _generator.get(_stateHistory.back().board, _stateHistory.back().turn);
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

    // update board count
    _boardCount[result.nextBoard]++;

    // update move logs
    _logs.push_back(result.notation);

    // update state history
    Color nextTurn = ~_stateHistory.back().turn;
    std::size_t nextMovesWithoutProgress = result.capture || result.pawnMoved ? 0 : _stateHistory.back().movesWithoutProgress + 1;
    const std::vector< std::shared_ptr< internal::ChessMove > >& nextPossibleMoves = _generator.get(result.nextBoard, nextTurn);

    // check if the next state is terminal
    CheckTerminal::MateStatus ms = CheckTerminal::isCheckmate(result.nextBoard, nextTurn, nextPossibleMoves.size());    

    bool isTerminal = false;
    Color winner = Color::NONE;

    // checkmate opponent
    if (ms == CheckTerminal::MateStatus::CHECKMATE) {
        isTerminal = true;
        winner = _stateHistory.back().turn;
    }

    // stalemate 
    else if (nextPossibleMoves.empty()) {
        isTerminal = true;
    }

    // by FIDE law, if 5-fold repetition occurs or 75 moves elapsed without progress, 
    // it's an automatic draw
    else if (_boardCount[result.nextBoard] == 5 || nextMovesWithoutProgress == 75) {
        isTerminal = true;
    } 
    
    // TODO: Dead position occurrs when neither player has a sequence of moves 
    // resulting in the opponent's king getting checked 

    // add new state to history
    _stateHistory.push_back({ result.nextBoard, isTerminal, nextTurn, winner, nextMovesWithoutProgress });

    // if the history exceeds capacity, delete least recent 
    if (_stateHistory.size() > _historySize) {
        _stateHistory.pop_front();
    }

    return true;
}


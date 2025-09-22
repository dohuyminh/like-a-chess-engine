#pragma once

#include "chess_board.h"
#include "moves/chess_move.h"
#include "moves/lru_movegen.h"
#include "fen.h"

#include <deque>
#include <unordered_map>

typedef std::shared_ptr< internal::ChessMove > Move;

class Arena {
public:
    
    Arena(std::size_t historySize = 1);

    // start with a custom board using FEN notation
    Arena(std::string fen, std::size_t historySize = 1);

    [[nodiscard]] inline Color winner() const {
        return _stateHistory.back().winner;
    }

    [[nodiscard]] inline Color turn() const {
        return _stateHistory.back().turn;
    }

    [[nodiscard]] inline std::size_t occurrence(const ChessBoard& board) {
        return _boardCount[board];
    }

    [[nodiscard]] inline std::size_t movesWithoutProgress() const {
        return _stateHistory.back().movesWithoutProgress;
    }

    [[nodiscard]] inline const std::vector< std::string >& logs() const {
        return _logs;
    }

    [[nodiscard]] inline const ChessBoard& currentBoard() const {
        return _stateHistory.back().board;
    }

    [[nodiscard]] inline std::vector< Move > getAllMoves() {
        // if the game has ended, there is no more move to play    
        // otherwise, return as normal
        return (_stateHistory.back().isTerminal) ? 
            std::vector< Move >() : 
            _generator.get(_stateHistory.back().board, _stateHistory.back().turn); 
    }

    bool performMove(const std::shared_ptr< internal::ChessMove >& mv); 

    inline bool rollBack() {
        // maximum rollback; denied
        if (_stateHistory.size() == 1) {
            return false;
        }
        
        // remove the state count for recent board
        --_boardCount[_stateHistory.back().board];

        _stateHistory.pop_back();
        return true;
    }

    [[nodiscard]] inline bool claimDraw() {
        // if the current state is repeated 3 times or there are 50 moves without progress 
        // mark the current state as terminal
        
        // if a game has already ended or the current state occurred less than 3 times 
        // and the moves withour progress is less than 50, cannot claim draw 
        if (_stateHistory.back().winner != Color::NONE || 
            (_stateHistory.back().movesWithoutProgress < 50 && _boardCount[_stateHistory.back().board] < 3)) {
            return false;
        }

        // actually true; time to end the game 
        _stateHistory.back().isTerminal = true;
        return true;
    }

    [[nodiscard]] inline bool resign() {
        // if the game has already ended, cannot resign
        if (_stateHistory.back().isTerminal) {
            return false;
        }

        // current player resigns, opponent wins
        _stateHistory.back().isTerminal = true;
        _stateHistory.back().winner     = ~_stateHistory.back().turn;
        return true;
    }

    [[nodiscard]] inline bool gameHasEnded() const {
        return _stateHistory.back().isTerminal;
    }

private:
    
    std::size_t _historySize;

    // board counter
    std::unordered_map< ChessBoard, std::size_t > _boardCount; 

    // move logs
    std::vector< std::string > _logs;  

    // keeps track of state
    std::deque< internal::State > _stateHistory;

    // for fast access to possible moves for each state
    // an average chess game may end at around 40-60 moves; we can take advantage 
    // of this statistics to cache around 30 moves so if it to be rolled back
    internal::LRULegalMoveGen _generator{ 30 };
};

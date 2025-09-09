#pragma once

#include "chess_board.h"
#include "moves/chess_move.h"
#include "moves/get_all_moves.h"

#include <deque>

struct State {
    const ChessBoard board;
    const bool isTerminal;
    const Color turn;
    const Color winner;
    const std::vector< std::shared_ptr< internal::ChessMove > > possibleMoves;
    const std::size_t movesWithoutProgress;

    State(
        const ChessBoard& board, 
        bool isTerminal, 
        Color turn, 
        Color winner, 
        std::size_t movesWithoutProgress
    );
};

class Arena {
public:
    
    Arena(std::size_t historySize = 1);

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

    void performMove(const std::shared_ptr< internal::ChessMove >& mv); 

private:
    
    std::size_t _historySize;

    // board counter
    std::unordered_map< ChessBoard, std::size_t > _boardCount; 

    // move logs
    std::vector< std::string > _logs;  

    // keeps track of state
    std::deque< State > _stateHistory;
};
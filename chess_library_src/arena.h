#pragma once

#include "chess_board.h"
#include "moves/chess_move.h"

class Arena {
public:
    
    Arena(bool setTime = false);

    [[nodiscard]] inline Color winner() const {
        return _winner;
    }

    [[nodiscard]] inline Color turn() const {
        return _turn;
    }

    [[nodiscard]] inline std::size_t occurrence(const ChessBoard& board) {
        return _boardCount[board];
    }

    [[nodiscard]] inline std::size_t movesWithoutProgress() const {
        return _movesWithoutProgress;
    }

    std::string performMove(const internal::ChessMove& mv);

        

private:
    
    // board information
    ChessBoard _currBoard;
    std::size_t _movesWithoutProgress;
    std::unordered_map< ChessBoard, std::size_t > _boardCount; 

    // player's information
    Color _turn;
    Color _winner;

    // move logs
    std::vector< std::string > _logs;  
};
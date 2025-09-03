#pragma once

#include "chess_board.h"
#include "moves/chess_move.h"
#include "moves/get_all_moves.h"

class Arena {
public:
    
    Arena();

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

    [[nodiscard]] inline const std::vector< std::string >& logs() const {
        return _logs;
    }

    [[nodiscard]] inline const ChessBoard& currentBoard() const {
        return _currBoard;
    }

    [[nodiscard]] inline Color turn() const {
        return _turn;
    }

    void performMove(const internal::ChessMove& mv);

    inline std::vector< std::shared_ptr< internal::ChessMove > > getAllMoves() const {
        return internal::getAllMoves(_currBoard, _turn);
    } 

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
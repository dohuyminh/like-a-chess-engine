#pragma once

#include "chess_board.h"
#include "check_terminal/checkmate.h"

#include <unordered_map>
#include <optional>


namespace ChessLibrary {
    
    enum class Color {
        WHITE, 
        BLACK, 
        UNDEFINED
    };

    /**
     * @brief A singleton class simulating a match of Chess. 
     * 
     * The object simulates a game of chess without enforcing any time constraint on deliberation. It keeps track of the number of occurrences 
     * of a particular state, the number of moves without progress (i.e. no capture is made or no pawn is moved). It also determines who the 
     * winner is at the end of the match. 
     */
    class Match {
    public:
        
        static Match& request();

        inline Color turn() const {
            return _turn;
        }

        inline const std::unordered_map<ChessBoard, size_t>& stateCount() const {
            return _stateCount;
        }

        inline const ChessBoard& getCurrState() const {
            return _currState;
        }

        inline bool gameHasEnded() const {
            return _winner != Color::UNDEFINED;
        }   

        inline Color winner() const {
            return _winner;
        }

        inline size_t count(const ChessBoard& board) {
            return _stateCount[board];
        }

        void reset();

    private:

        // the current board configuration
        ChessBoard _currState;         

        // keeps track of whose turn it is
        Color _turn;
        Color _winner;

        // keeps track of how many moves without progress has been made
        uint8_t _movesWithoutProgress;

        // keeps track of game state count 
        std::unordered_map<ChessBoard, size_t> _stateCount;

        Match();

        Match(Match const&)          = delete;
        void operator=(Match const&) = delete;
    };
}
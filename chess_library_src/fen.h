#pragma once 

#include "chess_board.h"
#include "moves/lru_movegen.h"

#include <exception>

namespace internal {

struct State {
    const ChessBoard board;
    bool isTerminal;
    const Color turn;
    Color winner;
    LRULegalMoveGen& generator;
    const std::size_t movesWithoutProgress;

    State(
        const ChessBoard& board, 
        Color turn, 
        std::size_t movesWithoutProgress,
        LRULegalMoveGen& generator
    );
};

class FENParsingException : public std::exception {
public:

    FENParsingException(const char* msg);

    FENParsingException(const char* msg, std::string fen, int idx);
    
    inline const char* what() const noexcept {
        return _msg.c_str();
    };

private:
    std::string _msg;
};

State loadStateFromFen(std::string fen, LRULegalMoveGen& generator); 

}

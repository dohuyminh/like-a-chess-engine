#pragma once

#include "../chess_board.h"

#include <unordered_map>
#include <utility>
#include <functional>

typedef std::pair<std::string, ChessBoard> MoveResult;


/**
 * @brief From a certain state, get all the possible moves for white/black player annotated in algebraic notation
 * 
 * @param state the current state of the game 
 * @return ```std::unordered_map<std::string, ChessBoard>``` A hashmap, with the key annotating the chess move
 * in algebraic notation (see https://en.wikipedia.org/wiki/Algebraic_notation_(chess)), and the value being 
 * the resulting state of the move 
 */
std::unordered_map<std::string, ChessBoard> getAllMovesAN(const ChessBoard& state, bool isWhiteTurn);

class AlgebraicNotation {
protected:
    virtual MoveResult performMove(const ChessBoard& state);         
};
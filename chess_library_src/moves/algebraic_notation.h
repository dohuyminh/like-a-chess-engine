#pragma once

#include "../chess_board.h"

#include <utility>
#include <string>

typedef std::pair<std::string, ChessBoard> MoveResult;

class AlgebraicNotation {
protected:
    virtual MoveResult performMove(const ChessBoard& state);         
};
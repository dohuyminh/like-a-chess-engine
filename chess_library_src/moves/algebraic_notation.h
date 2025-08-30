#pragma once

#include "../chess_board.h"

#include <utility>
#include <string>

typedef std::pair<std::string, ChessBoard> MoveResult;

namespace internal {

class AlgebraicNotation {
public:
    virtual ~AlgebraicNotation() = default;

protected:
    virtual MoveResult performMove(const ChessBoard& board);         
};

}
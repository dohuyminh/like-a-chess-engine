#pragma once

#include "queens_algebraic.h"
#include "knights_algebraic.h"
#include "underpromotion_algebraic.h"
#include "castling_algebraic.h"

#include <memory>

namespace internal {

MoveResult nextState(const ChessBoard& board, const std::shared_ptr< internal::ChessMove >& mv);

}
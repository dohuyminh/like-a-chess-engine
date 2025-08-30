#pragma once 

#include "chess_move.h"

#include <vector>
#include <memory>

namespace internal {

/**
 * @brief This function retrieves all possible moves for the current player given the current board of the chess board.
 * 
 * @param board The current board of the chess board
 * @param turn The color of the player
 * @param checkForCheck Whether to filter out moves that would leave the king in check (default: true)
 * @return A collection of possible moves for the current player
 */
std::vector<std::shared_ptr<ChessMove>> getAllMoves(
    const ChessBoard& board, 
    Color turn, 
    bool checkForCheck = true 
);

}
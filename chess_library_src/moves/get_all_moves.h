#pragma once 

#include "chess_move.h"

#include <vector>
#include <memory>

/**
 * @brief This function retrieves all possible moves for the current player given the current state of the chess board.
 * 
 * @param state The current state of the chess board
 * @param isWhiteTurn whether it is white player's turn to play (white: true, black: false)
 * @param checkForCheck whether to filter out moves that would leave the king in check (default: true)
 * @return A collection of possible moves for the current player
 */
std::vector<std::shared_ptr<ChessMove>> getAllMoves(
    const ChessBoard& state, 
    Color turn, 
    bool checkForCheck = true 
);
#include "checkmate.h"
#include "check.h"
#include "../moves/queens_move.h"

#include <stdexcept>
#include <iostream>

namespace CheckTerminal {
    MateStatus isCheckmate(const ChessBoard& board, bool kingIsWhite) {
        // if the king is not checked, it cannot be checkmate
        std::unordered_set<Coord2D> checkedPieces = kingIsChecked(board, kingIsWhite);
        if (checkedPieces.empty()) {
            return MateStatus::NONE;
        }

        Coord2D kingCoord = kingIsWhite ? board.whiteKingCoord() : board.blackKingCoord();

        // if there is only 1 piece checking the king, see if it can be captured or blocked 
        if (checkedPieces.size() == 1) {
            
            Coord2D checkingPieceCoord = *checkedPieces.begin();
            
            // check if the piece checking the king can be captured
            for (char col = 'a'; col <= 'h'; ++col) {
                for (int8_t row = 1; row <= 8; ++row) {
                    Coord2D pieceCoord(col, row);
                    Piece_t currPiece = board.getPiece(pieceCoord);
                    // if the piece is not of the same color, it can capture the checking piece
                    if (currPiece == static_cast<char>(ChessPiece::NONE) || (kingIsWhite != pieceIsWhite(currPiece))) {
                        continue;
                    }

                    // if the piece can reach the checking piece, it can capture it
                    if (__pieceCanReachSquare(board, pieceCoord, checkingPieceCoord)) {
                        return MateStatus::CHECK;
                    }
                }
            }

            // check if the piece checking the king can be blocked
            // if the piece is a knight, it cannot be blocked
            if (board.getPiece(checkingPieceCoord) != static_cast<char>(ChessPiece::BLACK_KNIGHT) && 
                board.getPiece(checkingPieceCoord) != static_cast<char>(ChessPiece::WHITE_KNIGHT)) {
                
                // find all the squares between the king and the checking piece
                int8_t deltaCol = (int8_t)checkingPieceCoord.col() - (int8_t)kingCoord.col(), 
                        deltaRow = checkingPieceCoord.row() - kingCoord.row();

                // either the king is checked horizontally/vertically/diagonally
                int8_t mag = std::max(std::abs(deltaCol), std::abs(deltaRow));
                Vec2D direction = Vec2D(deltaCol / mag, deltaRow / mag);
            
                // iterate blockable squares
                for (Coord2D blockableSquare = kingCoord + direction; blockableSquare != checkingPieceCoord; blockableSquare += direction) {
                    for (char col = 'a'; col <= 'h'; ++col) {
                        for (int8_t row = 1; row <= 8; ++row) {
                            Coord2D pieceCoord(col, row);
                            Piece_t currPiece = board.getPiece(pieceCoord);
                            // if the piece is not of the same color, it can block the checking piece
                            // for now, the king's movement is not considered 
                            if (pieceCoord == kingCoord || 
                                currPiece == static_cast<char>(ChessPiece::NONE) || 
                                (kingIsWhite != pieceIsWhite(currPiece))) {
                                continue;
                            }

                            // if the piece can reach the blockable square, it can block the checking piece
                            if (__pieceCanReachSquare(board, pieceCoord, blockableSquare)) {
                                return MateStatus::CHECK;
                            }
                        }
                    }
                }
            }
        }

        // if there are more pieces checking the king, or no pieces can capture/block them,
        // check if the king can move to a square that is not checked
        // check king's own move
        for (int dir = Direction::UP; dir <=Direction::DOWN_RIGHT; ++dir) {
            try {
                std::optional<ChessBoard> newState = QueensMove(kingIsWhite, static_cast<Direction>(dir), 1, kingCoord)(board);
                if (newState.has_value() && kingIsChecked(*newState, kingIsWhite).empty()) {
                    return MateStatus::CHECK; // the king can move to a square that is not checked
                }
            } catch (std::invalid_argument const&) {
                continue; // move is invalid; ignore
            }
        }       

        // if all else, king is checkmated
        return MateStatus::CHECKMATE;
    }
}
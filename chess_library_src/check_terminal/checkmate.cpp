#include "checkmate.h"
#include "check.h"
#include "../moves/get_all_moves.h"
#include "../moves/queens_move.h"
#include "../moves/knights_move.h"
#include "../moves/underpromotion.h"

#include <stdexcept>
#include <iostream>

namespace CheckTerminal {
    MateStatus isCheckmate(const ChessBoard& board, bool kingIsWhite) {
        // if the king is not checked, it cannot be checkmate
        std::unordered_set<Coord2D> checkingPieces = kingIsChecked(board, kingIsWhite);
        if (checkingPieces.empty()) {
            return MateStatus::NONE;
        }

        Coord2D kingCoord = kingIsWhite ? board.whiteKingCoord() : board.blackKingCoord();
        
        
        // if there is only 1 piece checking the king, see if it can be captured or blocked 
        if (checkingPieces.size() == 1) {
            
            Coord2D checkingPieceCoord = *checkingPieces.begin();
            Piece_t checkingPiece = board.getPiece(checkingPieceCoord);
            
            // get the squares a piece can get into to block/capture the checking piece
            std::unordered_set<Coord2D> captureBlockSet;
            captureBlockSet.insert(checkingPieceCoord);

            // if it's a knight checking the king, it may only be captured
            // otherwise blocking is possible
            if (checkingPiece != static_cast<Piece_t>(ChessPiece::BLACK_KNIGHT) && 
                checkingPiece != static_cast<Piece_t>(ChessPiece::WHITE_KNIGHT)) {

                // decompose the vector of which the piece is checking the king 
                // find all the squares between the king and the checking piece
                int8_t dx = (int8_t)checkingPieceCoord.col() - (int8_t)kingCoord.col(), 
                       dy = checkingPieceCoord.row() - kingCoord.row();

                // either the king is checked horizontally/vertically/diagonally
                int8_t mag = std::max(std::abs(dx), std::abs(dy));
                Vec2D direction = Vec2D(dx / mag, dy / mag);
                
                // iterate through blockable squares
                for (int m = 1; m < mag; ++m) {
                    captureBlockSet.insert(kingCoord + direction * m);
                }
            } 

            // get all the moves; whichever leads to the capture/blocking of the checking piece 
            // we also check if in the next state, the king is still in check or not
            std::vector<std::shared_ptr<ChessMove>> mvs = getAllMoves(board, kingIsWhite);
            for (const auto& mv: mvs) {

                // is the current move performed by the king piece?
                bool moveOnKing = false;
                
                // the piece's position after move 
                Coord2D postMove;
                
                // this is technically sound since castling is impossible if the king is in check
                if (std::dynamic_pointer_cast<QueensMove>(mv)) {
                    auto cast = std::dynamic_pointer_cast<QueensMove>(mv);
                    postMove = cast->origin() + cast->moveVec();
                    moveOnKing = board.whiteKingCoord() == cast->origin();
                } 
                else if (std::dynamic_pointer_cast<KnightsMove>(mv)) {
                    auto cast = std::dynamic_pointer_cast<KnightsMove>(mv);
                    postMove = cast->origin() + cast->moveVec();
                } 
                else {
                    auto cast = std::dynamic_pointer_cast<Underpromotion>(mv);
                    postMove = cast->origin() + cast->moveVec();
                }
                
                // if the move blocks/captures the checking piece, check if in the next state
                // the king is out of check
                if ((!moveOnKing && captureBlockSet.count(postMove)) || (moveOnKing && postMove == checkingPieceCoord)) {
                    std::optional<ChessBoard> nextState = (*mv)(board);
                    if (kingIsChecked(nextState.value(), kingIsWhite).empty()) {
                        return MateStatus::CHECK;
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
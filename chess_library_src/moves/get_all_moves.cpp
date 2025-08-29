#include "get_all_moves.h"

#include "queens_move.h"
#include "knights_move.h"
#include "underpromotion.h"
#include "castling.h"
#include "../check_terminal/check.h"
#include "../square_iterator/next_square_iterator.h"

#include <stdexcept>
#include <iostream>

static void insertAllPossibleMoves(
    const ChessBoard& board, 
    Coord2D pieceCoord, 
    std::vector<std::shared_ptr<internal::ChessMove>>& moves);

static void generateMove(
    Coord2D origin, 
    Vec2D mv, 
    ChessPiece pieceAtOrigin, 
    std::vector<std::shared_ptr<internal::ChessMove>>& moves);

namespace internal {

std::vector<std::shared_ptr<ChessMove>> getAllMoves(
    const ChessBoard& board,
    Color turn,
    bool checkForCheck
) {
    
    using namespace internal::CheckTerminal;

    std::vector<std::shared_ptr<ChessMove>> moves;

    // iterate all pieces on the board
    for (char col = 'a'; col <= 'h'; ++col) {
        for (int8_t row = 1; row <= 8; ++row) {
        
            Coord2D pieceCoord(col, row);
            ChessPiece piece = board.getPiece(pieceCoord);
            
            // skip all irrelevant pieces
            if (turn != piece.color()) {
                continue;
            }
            
            // for each piece, insert all possible moves
            insertAllPossibleMoves(board, pieceCoord, moves);
        }
    }

    // cover castling 
    if (kingIsChecked(board, turn).empty()) {
        
        bool isWhiteTurn = turn == Color::WHITE;
        
        bool leftCastling = board.castling(turn, true);
        bool rightCastling = board.castling(turn, false);
        
        Coord2D kingCoord;
        for (char col = 'a'; col <= 'h'; ++col) {
            for (int8_t row = 1; row <= 8; ++row) {
                Coord2D coord(col, row);
                ChessPiece piece = board.getPiece(coord); 
                if (piece.isKing() && piece.color() == turn) {
                    kingCoord = coord; break;
                }
            }
        }

        ChessPiece rook = isWhiteTurn ? ChessPiece::WHITE_ROOK : ChessPiece::BLACK_ROOK; 

        if (leftCastling) {
            bool canPerform = true;
            Vec2D mvVec = isWhiteTurn ? Vec2D(-1, 0) : Vec2D(1, 0);

            for (Coord2D iter = kingCoord + mvVec; board.getPiece(iter) != rook; iter = iter + mvVec) {
                ChessPiece iterPiece = board.getPiece(iter);
                if (!iterPiece.isNone()) {
                    canPerform = false; break;
                }
            }
            
            if (canPerform) moves.push_back(std::make_shared<Castling>(turn, true));
        }

        if (rightCastling) {
            bool canPerform = true;
            Vec2D mvVec = isWhiteTurn ? Vec2D(1, 0) : Vec2D(-1, 0);

            for (Coord2D iter = kingCoord + mvVec; board.getPiece(iter) != rook; iter = iter + mvVec) {
                ChessPiece iterPiece = board.getPiece(iter);
                if (!iterPiece.isNone()) {
                    canPerform = false; break;
                }
            }

            if (canPerform) moves.push_back(std::make_shared<Castling>(turn, false));
        }
    }

    // if checkForCheck is true, filter out moves that result in check
    if (checkForCheck) {
        
        moves.erase(std::remove_if(moves.begin(), moves.end(), [&board, turn](const std::shared_ptr<ChessMove>& move) {
            // apply the move to the board
            std::optional<ChessBoard> newState = (*move)(board);
            auto check = kingIsChecked(newState.value(), turn);
            // check if the king is checked after the move
            return !check.empty();
        }), moves.end());

    }

    return moves;
}

}
 
static void insertAllPossibleMoves(const ChessBoard& board, Coord2D pieceCoord, std::vector<std::shared_ptr<internal::ChessMove>>& moves) {
    
    ChessPiece pieceAtCoord = board.getPiece(pieceCoord);

    std::vector<Coord2D> possibleSquares = findAvailableSquares(board, pieceCoord);
    for (Coord2D dest: possibleSquares) {
        
        int originX = pieceCoord.col() - 'a', destX = dest.col() - 'a', 
            originY = pieceCoord.row(), destY = dest.row();
        
        int dx = destX - originX, dy = destY - originY;
        Vec2D mv(dx, dy);
        if (pieceAtCoord.isBlack()) mv *= -1;

        generateMove(pieceCoord, mv, pieceAtCoord, moves);    
    }
}

static void generateMove(Coord2D origin, Vec2D mv, ChessPiece pieceAtOrigin, std::vector<std::shared_ptr<internal::ChessMove>>& moves) {
    
    bool isWhiteTurn = pieceAtOrigin.isWhite();
    
    bool isKnight = pieceAtOrigin.isKnight();
    if (isKnight) {
        moves.push_back(std::make_shared<internal::KnightsMove>(pieceAtOrigin.color(), mv, origin));
        return;
    }

    uint8_t magnitude = std::max(std::abs(mv.mvCol()), std::abs(mv.mvRow()));
    Vec2D dirVec(mv.mvCol() / magnitude, mv.mvRow() / magnitude);
    
    auto dir = static_cast<Direction>(std::find(vecMap, vecMap + 8, dirVec) - vecMap);
    moves.push_back(std::make_shared<internal::QueensMove>(pieceAtOrigin.color(), dir, magnitude, origin));


    // if it's a pawn moving to the last rank, cover the cases of underpromotion
    ChessPiece pawn = isWhiteTurn ? ChessPiece::WHITE_PAWN : ChessPiece::BLACK_PAWN;
    int8_t row = isWhiteTurn ? 7 : 2;
    bool pawnAtOrigin = pieceAtOrigin == pawn;

    if (pawnAtOrigin && origin.row() == row) {
        ChessPiece  p1 = (isWhiteTurn) ? ChessPiece::WHITE_ROOK : ChessPiece::BLACK_ROOK, 
                    p2 = (isWhiteTurn) ? ChessPiece::WHITE_BISHOP : ChessPiece::BLACK_BISHOP, 
                    p3 = (isWhiteTurn) ? ChessPiece::WHITE_KNIGHT : ChessPiece::BLACK_KNIGHT;
        moves.push_back(std::make_shared<internal::Underpromotion>(pieceAtOrigin.color(), dir, origin, p1));
        moves.push_back(std::make_shared<internal::Underpromotion>(pieceAtOrigin.color(), dir, origin, p2));
        moves.push_back(std::make_shared<internal::Underpromotion>(pieceAtOrigin.color(), dir, origin, p3));
    }
}
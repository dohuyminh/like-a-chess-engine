#include "get_all_moves.h"

#include "queens_move.h"
#include "knights_move.h"
#include "underpromotion.h"
#include "castling.h"
#include "../check_terminal/check.h"
#include "../square_iterator/next_square_iterator.h"

#include <stdexcept>
#include <iostream>

static void insertAllPossibleMoves(const ChessBoard& board, Coord2D pieceCoord, bool isWhiteTurn, std::vector<std::shared_ptr<ChessMove>>& moves);
static void generateMove(Coord2D origin, bool isWhiteTurn, Vec2D mv, Piece_t pieceAtOrigin, std::vector<std::shared_ptr<ChessMove>>& moves);

std::vector<std::shared_ptr<ChessMove>> getAllMoves(
    const ChessBoard& board,
    bool isWhiteTurn,
    bool checkForCheck
) {
    
    using namespace CheckTerminal;

    auto filterPiece = isWhiteTurn ? pieceIsWhite : pieceIsBlack;
    std::vector<std::shared_ptr<ChessMove>> moves;

    // iterate all pieces on the board
    for (char col = 'a'; col <= 'h'; ++col) {
        for (int8_t row = 1; row <= 8; ++row) {
        
            Coord2D pieceCoord(col, row);
            Piece_t piece = board.getPiece(pieceCoord);
            
            // skip all irrelevant pieces
            if (!filterPiece(piece)) {
                continue;
            }
            
            // for each piece, insert all possible moves
            insertAllPossibleMoves(board, pieceCoord, isWhiteTurn, moves);
        }
    }

    // cover castling 
    if (kingIsChecked(board, isWhiteTurn).empty()) {
        bool leftCastling = (isWhiteTurn && board.whiteLeftCastling()) || (!isWhiteTurn && board.blackLeftCastling());
        bool rightCastling = (isWhiteTurn && board.whiteRightCastling()) || (!isWhiteTurn && board.blackRightCastling());
        
        Coord2D kingCoord = isWhiteTurn ? board.whiteKingCoord() : board.blackKingCoord();
        Piece_t rook = isWhiteTurn ? static_cast<Piece_t>(ChessPiece::WHITE_ROOK) : 
                                     static_cast<Piece_t>(ChessPiece::BLACK_ROOK); 

        if (leftCastling) {
            bool canPerform = true;
            Vec2D mvVec = isWhiteTurn ? Vec2D(-1, 0) : Vec2D(1, 0);

            for (Coord2D iter = kingCoord + mvVec; board.getPiece(iter) != rook; iter = iter + mvVec) {
                Piece_t iterPiece = board.getPiece(iter);
                if (iterPiece != static_cast<Piece_t>(ChessPiece::NONE)) {
                    canPerform = false; break;
                }
            }
            
            if (canPerform) moves.push_back(std::make_shared<Castling>(isWhiteTurn, true));
        }

        if (rightCastling) {
            bool canPerform = true;
            Vec2D mvVec = isWhiteTurn ? Vec2D(1, 0) : Vec2D(-1, 0);

            for (Coord2D iter = kingCoord + mvVec; board.getPiece(iter) != rook; iter = iter + mvVec) {
                Piece_t iterPiece = board.getPiece(iter);
                if (iterPiece != static_cast<Piece_t>(ChessPiece::NONE)) {
                    canPerform = false; break;
                }
            }

            if (canPerform) moves.push_back(std::make_shared<Castling>(isWhiteTurn, false));
        }
    }

    // if checkForCheck is true, filter out moves that result in check
    if (checkForCheck) {
        
        moves.erase(std::remove_if(moves.begin(), moves.end(), [&board, isWhiteTurn](const std::shared_ptr<ChessMove>& move) {
            // apply the move to the board
            std::optional<ChessBoard> newState = (*move)(board);
            auto check = kingIsChecked(newState.value(), isWhiteTurn);
            // check if the king is checked after the move
            return !check.empty();
        }), moves.end());

    }

    return moves;
}

static void insertAllPossibleMoves(const ChessBoard& board, Coord2D pieceCoord, bool isWhiteTurn, std::vector<std::shared_ptr<ChessMove>>& moves) {
    
    Piece_t pieceAtCoord = board.getPiece(pieceCoord);
    bool isKnight = pieceAtCoord == static_cast<Piece_t>(ChessPiece::WHITE_KNIGHT) || 
                    pieceAtCoord == static_cast<Piece_t>(ChessPiece::BLACK_KNIGHT);

    std::vector<Coord2D> possibleSquares = findAvailableSquares(board, pieceCoord);
    for (Coord2D dest: possibleSquares) {
        
        int originX = pieceCoord.col() - 'a', destX = dest.col() - 'a', 
            originY = pieceCoord.row(), destY = dest.row();
        
        int dx = destX - originX, dy = destY - originY;
        Vec2D mv(dx, dy);
        if (!isWhiteTurn) mv *= -1;

        generateMove(pieceCoord, isWhiteTurn, mv, pieceAtCoord, moves);    
    }
}

static void generateMove(Coord2D origin, bool isWhiteTurn, Vec2D mv, Piece_t pieceAtOrigin, std::vector<std::shared_ptr<ChessMove>>& moves) {
    bool isKnight = pieceAtOrigin == static_cast<Piece_t>(ChessPiece::WHITE_KNIGHT) || 
                    pieceAtOrigin == static_cast<Piece_t>(ChessPiece::BLACK_KNIGHT);
    if (isKnight) {
        moves.push_back(std::make_shared<KnightsMove>(isWhiteTurn, mv, origin));
        return;
    }

    uint8_t magnitude = std::max(std::abs(mv.mvCol()), std::abs(mv.mvRow()));
    Vec2D dirVec(mv.mvCol() / magnitude, mv.mvRow() / magnitude);
    
    Direction dir = static_cast<Direction>(std::find(vecMap, vecMap + 8, dirVec) - vecMap);
    moves.push_back(std::make_shared<QueensMove>(isWhiteTurn, dir, magnitude, origin));


    // if it's a pawn moving to the last rank, cover the cases of underpromotion
    Piece_t pawn = isWhiteTurn ? static_cast<Piece_t>(ChessPiece::WHITE_PAWN) : static_cast<Piece_t>(ChessPiece::BLACK_PAWN);
    int8_t row = isWhiteTurn ? 7 : 2;
    bool pawnAtOrigin = pieceAtOrigin == pawn;

    if (pawnAtOrigin && origin.row() == row) {
        ChessPiece  p1 = (isWhiteTurn) ? ChessPiece::WHITE_ROOK : ChessPiece::BLACK_ROOK, 
                    p2 = (isWhiteTurn) ? ChessPiece::WHITE_BISHOP : ChessPiece::BLACK_BISHOP, 
                    p3 = (isWhiteTurn) ? ChessPiece::WHITE_KNIGHT : ChessPiece::BLACK_KNIGHT;
        moves.push_back(std::make_shared<Underpromotion>(isWhiteTurn, dir, origin, p1));
        moves.push_back(std::make_shared<Underpromotion>(isWhiteTurn, dir, origin, p2));
        moves.push_back(std::make_shared<Underpromotion>(isWhiteTurn, dir, origin, p3));
    }
}
#include "get_all_moves.h"

#include "queens_move.h"
#include "knights_move.h"
#include "underpromotion.h"
#include "castling.h"
#include "../check_terminal/check.h"

#include <stdexcept>

static void insertAllPossibleMoves(const ChessBoard& board, Coord2D pieceCoord, bool isWhiteTurn, std::vector<std::shared_ptr<ChessMove>>& moves);

std::vector<std::shared_ptr<ChessMove>> getAllMoves(
    const ChessBoard& board,
    bool isWhiteTurn,
    bool checkForCheck
) {
    auto filterPiece = isWhiteTurn ? pieceIsWhite : pieceIsBlack;
    std::vector<std::shared_ptr<ChessMove>> moves;

    // iterate all pieces on the board
    for (char col = 'A'; col <= 'H'; ++col) {
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

    // if checkForCheck is true, filter out moves that result in check
    if (checkForCheck) {
        std::remove_if(moves.begin(), moves.end(), [&board, isWhiteTurn](const std::shared_ptr<ChessMove>& move) {
            // apply the move to the board
            std::optional<ChessBoard> newState = (*move)(board);
            if (!newState.has_value()) {
                return true; // move is invalid
            }
            // check if the king is checked after the move
            return !CheckTerminal::kingIsChecked(*newState, isWhiteTurn).empty();
        });
    }

    return moves;
}

#include <iostream>
static void insertAllPossibleMoves(const ChessBoard& board, Coord2D pieceCoord, bool isWhiteTurn, std::vector<std::shared_ptr<ChessMove>>& moves) {
    Piece_t piece = board.getPiece(pieceCoord);
    if (piece == static_cast<char>(ChessPiece::NONE)) {
        return; // no piece at this square
    }

    if (piece == static_cast<char>(ChessPiece::WHITE_PAWN) || piece == static_cast<char>(ChessPiece::BLACK_PAWN)) 
    {
        // pedantic check for pawn's position
        if ((isWhiteTurn && pieceCoord.row() == 8) || (!isWhiteTurn && pieceCoord.row() == 1)) {
            throw std::logic_error("FATAL ERROR: PAWNS REACHING LAST ROW SHOULD HAVE BEEN PROMOTED");
        }
        
        // check for pawn's forward movement
        Vec2D forward = (isWhiteTurn) ? Vec2D(0, 1) : Vec2D(0, -1);
        if (board.getPiece(pieceCoord + forward) == static_cast<char>(ChessPiece::NONE)){
            moves.push_back(std::make_shared<QueensMove>(QueensMove(isWhiteTurn, Direction::UP, 1, pieceCoord)));
            
            bool isOrigin = (isWhiteTurn && pieceCoord.row() == 2) || (!isWhiteTurn && pieceCoord.row() == 7);
            if (isOrigin && board.getPiece(pieceCoord + forward * 2) == static_cast<char>(ChessPiece::NONE)) {
                moves.push_back(std::make_shared<QueensMove>(QueensMove(isWhiteTurn, Direction::UP, 2, pieceCoord)));
            } 
        }
        
        // check for diagonal move (either by capturing another piece or en passant)
        // left diagonal
        Vec2D left = (isWhiteTurn) ? Vec2D(-1, 1) : Vec2D(1, -1);
        bool canMoveLeft = (isWhiteTurn && pieceCoord.col() > 'A') || (!isWhiteTurn && pieceCoord.col() < 'H');
        if (canMoveLeft && CheckTerminal::__pieceCanReachSquare(board, pieceCoord, pieceCoord + left)) {
            moves.push_back(std::make_shared<QueensMove>(QueensMove(isWhiteTurn, Direction::UP_LEFT, 1, pieceCoord)));
        }

        // right diagonal
        Vec2D right = (isWhiteTurn) ? Vec2D(1, 1) : Vec2D(-1, -1);
        bool canMoveRight = (isWhiteTurn && pieceCoord.col() < 'H') || (!isWhiteTurn && pieceCoord.col() > 'A');
        if (canMoveRight && CheckTerminal::__pieceCanReachSquare(board, pieceCoord, pieceCoord + right)) {
            moves.push_back(std::make_shared<QueensMove>(QueensMove(isWhiteTurn, Direction::UP_RIGHT, 1, pieceCoord)));
        }
    }
        
    bool isKnight = piece == static_cast<char>(ChessPiece::WHITE_KNIGHT) || piece == static_cast<char>(ChessPiece::BLACK_KNIGHT);
    bool isKing = piece == static_cast<char>(ChessPiece::WHITE_KING) || piece == static_cast<char>(ChessPiece::BLACK_KING);
    if (isKnight || isKing)
    {
        const Vec2D* mvs = isKnight ? knightsMoveVec : vecMap;
        for (int8_t mvIdx = 0; mvIdx < 8; ++mvIdx) {
            try {
                Coord2D dest = pieceCoord + mvs[mvIdx] * (isWhiteTurn ? 1 : -1);
                Piece_t pieceAtDest = board.getPiece(dest);
                if (pieceAtDest == static_cast<Piece_t>(ChessPiece::NONE) || 
                    isWhiteTurn != pieceIsWhite(pieceAtDest)) {
                
                    std::cout << "Inserting vector move: " << (std::string)mvs[mvIdx] << " for piece at " << (std::string)pieceCoord << std::endl;
                    std::shared_ptr<ChessMove> move;
                    if (isKnight) {
                        move = std::make_shared<KnightsMove>(KnightsMove(isWhiteTurn, mvs[mvIdx], pieceCoord));
                    } else {
                        move = std::make_shared<QueensMove>(QueensMove(isWhiteTurn, static_cast<Direction>(mvIdx), 1, pieceCoord));
                    }

                    moves.push_back(move);
                }
            } catch (std::invalid_argument const& e) {
                continue;
            }
        }
    }

    bool isQueen = piece == static_cast<char>(ChessPiece::WHITE_QUEEN) || piece == static_cast<char>(ChessPiece::BLACK_QUEEN);
    bool isBishop = piece == static_cast<char>(ChessPiece::WHITE_BISHOP) || piece == static_cast<char>(ChessPiece::BLACK_BISHOP);
    if (isQueen || isBishop)
    {
        // only check for diagonal
        Direction vecs[4] = { Direction::UP_LEFT, Direction::UP_RIGHT, Direction::DOWN_LEFT, Direction::DOWN_RIGHT };
        bool iterCheck[4] = { true, true, true, true };
        int8_t nSteps = 1;
        do {
            
            for (int8_t i = 0; i < 4; ++i) {
                char   currCol = pieceCoord.col() + vecMap[vecs[i]].mvCol() * nSteps * (isWhiteTurn ? 1 : -1);
                int8_t currRow = pieceCoord.row() + vecMap[vecs[i]].mvRow() * nSteps * (isWhiteTurn ? 1 : -1); 
                iterCheck[i] = 'A' <= currCol && currCol <= 'H' && 1 <= currRow && currRow <= 8;
                if (iterCheck[i]) {
                    Coord2D curr(currCol, currRow);
                    Piece_t currPiece = board.getPiece(curr);

                    // if current square has a piece of the same color
                    if (currPiece != static_cast<Piece_t>(ChessPiece::NONE) && 
                        isWhiteTurn == pieceIsWhite(currPiece)) {
                        
                        iterCheck[i] = false; continue;
                    }

                    // insert it to list of possible moves; stop if it's a capture
                    moves.push_back(std::make_shared<QueensMove>(QueensMove(isWhiteTurn, vecs[i], nSteps, pieceCoord)));
                }
            }
            ++nSteps;

        } while (iterCheck[0] || iterCheck[1] || iterCheck[2] || iterCheck[3]);
    }

    bool isRook = piece == static_cast<char>(ChessPiece::WHITE_ROOK) || piece == static_cast<char>(ChessPiece::BLACK_ROOK);
    if (isQueen || isRook)
    {
        // only check for diagonal
        Direction vecs[4] = { Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT };
        bool iterCheck[4] = { true, true, true, true };
        int8_t nSteps = 1;
        do {
            
            for (int8_t i = 0; i < 4; ++i) {
                char   currCol = pieceCoord.col() + vecMap[vecs[i]].mvCol() * nSteps * (isWhiteTurn ? 1 : -1);
                int8_t currRow = pieceCoord.row() + vecMap[vecs[i]].mvRow() * nSteps * (isWhiteTurn ? 1 : -1); 
                iterCheck[i] = 'A' <= currCol && currCol <= 'H' && 1 <= currRow && currRow <= 8;
                if (iterCheck[i]) {
                    Coord2D curr(currCol, currRow);
                    Piece_t currPiece = board.getPiece(curr);

                    // if current square has a piece of the same color
                    if (currPiece != static_cast<Piece_t>(ChessPiece::NONE) && 
                        isWhiteTurn == pieceIsWhite(currPiece)) {
                        
                        iterCheck[i] = false; continue;
                    }

                    // insert it to list of possible moves; stop if it's a capture
                    moves.push_back(std::make_shared<QueensMove>(QueensMove(isWhiteTurn, vecs[i], nSteps, pieceCoord)));
                }
            }
            ++nSteps;

        } while (iterCheck[0] || iterCheck[1] || iterCheck[2] || iterCheck[3]);
    }
}

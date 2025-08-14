#include "algebraic_notation.h"
#include "get_all_moves.h"
#include "queens_move.h"
#include "knights_move.h"
#include "castling.h"
#include "underpromotion.h"

#include "../check_terminal/checkmate.h"
#include "../check_terminal/check.h"

#include <stdexcept>
#include <cctype>

static std::string generateMoveNotation(
    const ChessBoard& state, 
    const ChessMove& mv,
    std::string resolveAmbiguity);

std::unordered_map<std::string, ChessBoard> getAllMovesAN(const ChessBoard& state, bool isWhiteTurn) {
    
    // get all possible moves from state
    std::vector<std::shared_ptr<ChessMove>> mvs = getAllMoves(state, isWhiteTurn);

    std::unordered_map<std::string, ChessBoard> res;
    std::unordered_map<std::optional<Coord2D>, std::vector<std::shared_ptr<ChessMove>>> trackAmbiguity;

    // for each move, figure out which moves are ambiguous (i.e. sharing the same destination)
    for (const auto& mv: mvs) {
        // get the destination square for the move
        if (std::shared_ptr<QueensMove> cast = std::dynamic_pointer_cast<QueensMove>(mv)) {
            Coord2D dest = cast->origin() + cast->moveVec();
            trackAmbiguity[dest].push_back(mv);
        } else if (std::shared_ptr<KnightsMove> cast = std::dynamic_pointer_cast<KnightsMove>(mv)) {
            Coord2D dest = cast->origin() + cast->moveVec();
            trackAmbiguity[dest].push_back(mv);
        } else if (std::shared_ptr<Underpromotion> cast = std::dynamic_pointer_cast<Underpromotion>(mv)) {
            Coord2D dest = cast->origin() + cast->moveVec();
            trackAmbiguity[dest].push_back(mv);
        } else {
            trackAmbiguity[std::nullopt].push_back(mv);
        }
    }

    // for each move set, create 
    for (const auto& [k, v]: trackAmbiguity) {

        std::string ambigResolve;
        bool ambigCol = false;
        bool ambigRow = false;

        // resolve if more than 2 pieces can reach the same square
        if (k.has_value() && v.size() > 1) {
            
            uint8_t colMap = 0, rowMap = 0;
            
            for (const std::shared_ptr<ChessMove> mv: v) {
                int colIdx, rowIdx;
                
                if (std::shared_ptr<QueensMove> cast = std::dynamic_pointer_cast<QueensMove>(mv)) {
                    colIdx = cast->origin().col() - 'A';
                    rowIdx = cast->origin().row() - 1;        
                } else if (std::shared_ptr<KnightsMove> cast = std::dynamic_pointer_cast<KnightsMove>(mv)) {
                    colIdx = cast->origin().col() - 'A';
                    rowIdx = cast->origin().row() - 1;
                } else if (std::shared_ptr<Underpromotion> cast = std::dynamic_pointer_cast<Underpromotion>(mv)) {
                    colIdx = cast->origin().col() - 'A';
                    rowIdx = cast->origin().row() - 1;
                } else {
                    // castling should not have been inserted here
                    throw std::logic_error("FATAL: CASTLING MOVE INSERTED IN WRONG SET OF MOVES");
                }

                // see if we have seen the row/column before
                ambigCol = ambigCol || (colMap >> colIdx) & 1;
                ambigRow = ambigRow || (rowMap >> rowIdx) & 1; 
            
                // memoize 
                colMap |= 1 << colIdx;
                rowMap |= 1 << rowIdx;
            }

        }

        for (const auto& mv: v) {
            std::optional<ChessBoard> nextState = (*mv)(state);
            
            // resolve ambiguity if it exists
            if (k != std::nullopt && v.size() > 1) {
                
                // attempt to get the coordinate of origin piece
                Coord2D origin;
                
                if (std::shared_ptr<QueensMove> cast = std::dynamic_pointer_cast<QueensMove>(mv)) {
                    origin = cast->origin();            
                } else if (std::shared_ptr<KnightsMove> cast = std::dynamic_pointer_cast<KnightsMove>(mv)) {
                    origin = cast->origin();
                } else if (std::shared_ptr<Underpromotion> cast = std::dynamic_pointer_cast<Underpromotion>(mv)) {
                    origin = cast->origin();
                } else {
                    // castling should not have been inserted here
                    throw std::logic_error("FATAL: CASTLING MOVE INSERTED IN WRONG SET OF MOVES");
                }
            
                // if the pieces do not match columns -> resolve by columns by default
                if (!ambigCol) {
                    ambigResolve.push_back(std::tolower(origin.col()));
                } 
                // if the pieces do not match rows -> resolve by rows
                else if (!ambigRow) {
                    ambigResolve.push_back(origin.row() + '0');
                }
                // otherwise, perform double disambiguation
                else {
                    ambigResolve.push_back(std::tolower(origin.col()));
                    ambigResolve.push_back(origin.row() + '0');
                }
            }

            std::string an = generateMoveNotation(state, *mv, ambigResolve);

            // if the move causes the next state to check/checkmate the opponent's king
            CheckTerminal::MateStatus ms = CheckTerminal::isCheckmate(nextState.value(), !isWhiteTurn);
            if (ms == CheckTerminal::MateStatus::CHECKMATE) {
                an.push_back('+');
                an.push_back('+');
            } else if (ms == CheckTerminal::MateStatus::CHECK) {
                an.push_back('+');
            }

            res[an] = nextState.value();
        }
    } 

    return res;
}

static std::string generateMoveNotation(
    const ChessBoard& state, 
    const ChessMove& mv,
    std::string resolveAmbiguity) {
    
    std::string res;

    // if the move is a queen's move
    if (dynamic_cast<const QueensMove *>(&mv)) {
        const QueensMove* qCast = dynamic_cast<const QueensMove *>(&mv);
        
        // get the piece to move 
        Piece_t pieceAtOrigin = state.getPiece(qCast->origin());
        
        switch (pieceAtOrigin)
        {
        case static_cast<Piece_t>(ChessPiece::WHITE_KING):
        case static_cast<Piece_t>(ChessPiece::BLACK_KING):
            res.push_back('K');
            break;
        
        case static_cast<Piece_t>(ChessPiece::WHITE_ROOK):
        case static_cast<Piece_t>(ChessPiece::BLACK_ROOK):
            res.push_back('R');
            break;

        case static_cast<Piece_t>(ChessPiece::WHITE_BISHOP):
        case static_cast<Piece_t>(ChessPiece::BLACK_BISHOP):
            res.push_back('B');
            break;

        default:
            break;
        }
        
        // resolve any ambiguity
        res += resolveAmbiguity;

        // confirm whether the piece captures something
        Coord2D dest = qCast->origin() + qCast->moveVec();
        bool originPieceIsWhitePawn = pieceAtOrigin == static_cast<Piece_t>(ChessPiece::WHITE_PAWN);
        bool originPieceIsBlackPawn = pieceAtOrigin == static_cast<Piece_t>(ChessPiece::BLACK_PAWN); 
        bool originPieceIsPawn = originPieceIsBlackPawn || originPieceIsWhitePawn;           
        
        if (state.getPiece(dest) != static_cast<Piece_t>(ChessPiece::NONE) || 
            (
                (originPieceIsWhitePawn && dest == state.whiteEnpassant()) || 
                (originPieceIsBlackPawn && dest == state.blackEnpassant())
            )
        ) {
            if (originPieceIsPawn) res.push_back(std::tolower(qCast->origin().col()));    
            res.push_back('x');
        }

        // insert the destination square 
        res.push_back(std::tolower(dest.col()));
        res.push_back(dest.row() + '0');

        // if it's a pawn reaching the last row -> promote to Queen
        if ((originPieceIsWhitePawn && dest.row() == 8) || (originPieceIsBlackPawn && dest.row() == 1)) {
            res.push_back('Q');
        }
    }

    // if the move is a knight's move
    else if (dynamic_cast<const KnightsMove *>(&mv)) {
        
        const KnightsMove* nCast = dynamic_cast<const KnightsMove *>(&mv);

        // knight's move notation
        res.push_back('N');

        // resolve ambiguity
        res += resolveAmbiguity;

        // confirming whether the piece captures something 
        Coord2D dest = nCast->origin() + nCast->moveVec();
        if (state.getPiece(dest) != static_cast<Piece_t>(ChessPiece::NONE)) {
            res.push_back('x');
        }

        // insert destination square
        res.push_back(std::tolower(dest.col()));
        res.push_back(dest.row() + '0');
    }

    // if the move is an underpromotion
    else if (dynamic_cast<const Underpromotion *>(&mv)) {
        
        const Underpromotion* uCast = dynamic_cast<const Underpromotion *>(&mv);

        // resolve ambiguity
        res += resolveAmbiguity;

        // confirming whether the piece captures something
        Coord2D dest = uCast->origin() + uCast->moveVec();
        if (state.getPiece(dest) != static_cast<Piece_t>(ChessPiece::NONE)) {
            res.push_back(std::tolower(uCast->origin().col()));
            res.push_back('x');
        }

        // insert destination square
        res.push_back(std::tolower(dest.col()));
        res.push_back(dest.row() + '0');

        // denote the promotion
        if (
            uCast->promotePiece() == ChessPiece::WHITE_ROOK || 
            uCast->promotePiece() == ChessPiece::BLACK_ROOK) {
            
            res.push_back('R');
        } else if (
            uCast->promotePiece() == ChessPiece::WHITE_KNIGHT || 
            uCast->promotePiece() == ChessPiece::BLACK_KNIGHT) {

            res.push_back('N');
        } else {
            res.push_back('B');
        }
    }

    // if the move is a castling move
    else {
        const Castling& qCast = dynamic_cast<const Castling &>(mv);
        res += (qCast.moveLeft() ? "0-0-0" : "0-0");
    }

    return res;
}

#include "queens_algebraic.h"
#include "../check_terminal/checkmate.h"

#include <stdexcept>

QueensAlgebraic::QueensAlgebraic(QueensMove mv, std::string resolveAmbiguity) noexcept :
    _mv(mv), _resolveAmbiguity(resolveAmbiguity) {}

MoveResult QueensAlgebraic::performMove(const ChessBoard& state) {

    using namespace CheckTerminal;

    // ensure the given move is valid 
    std::optional<ChessBoard> nextState = _mv(state);
    if (!nextState.has_value()) {
        throw std::invalid_argument("The queen's move is not valid for the given state");
    }
    
    std::string an;
    Piece_t pieceAtOrigin = state.getPiece(_mv.origin());
    // depends on which piece is being moved, note it at the beginning of the move's name
    switch (pieceAtOrigin) {
        // King: K
        case static_cast<Piece_t>(ChessPiece::WHITE_KING):
        case static_cast<Piece_t>(ChessPiece::BLACK_KING):
            an.push_back('K');
            break;

        // Queen: Q
        case static_cast<Piece_t>(ChessPiece::WHITE_QUEEN):
        case static_cast<Piece_t>(ChessPiece::BLACK_QUEEN):
            an.push_back('Q');
            break;

        // Rook: R
        case static_cast<Piece_t>(ChessPiece::WHITE_ROOK):
        case static_cast<Piece_t>(ChessPiece::BLACK_ROOK):
            an.push_back('R');
            break;

        // Bishop: B
        case static_cast<Piece_t>(ChessPiece::WHITE_BISHOP):
        case static_cast<Piece_t>(ChessPiece::BLACK_BISHOP):
            an.push_back('B');
            break;

        // Pawn: [empty]
        default:
            break;
    }

    // resolve any ambiguity
    an += _resolveAmbiguity;
    
    // confirm if the piece captured something
    Coord2D dest = _mv.origin() + _mv.moveVec();
    bool originPieceIsWhitePawn = pieceAtOrigin == static_cast<Piece_t>(ChessPiece::WHITE_PAWN);
    bool originPieceIsBlackPawn = pieceAtOrigin == static_cast<Piece_t>(ChessPiece::BLACK_PAWN); 
    bool originPieceIsPawn = originPieceIsBlackPawn || originPieceIsWhitePawn;

    if (state.getPiece(dest) != static_cast<Piece_t>(ChessPiece::NONE) ||
        (originPieceIsWhitePawn && dest == state.whiteEnpassant()) || 
        (originPieceIsBlackPawn && dest == state.blackEnpassant()) ) {
        
        an.push_back('x');
    }

    // insert destination square 
    an.push_back(dest.col());
    an.push_back(dest.row() + '0');

    // if it's a pawn reaching the last row, promote it to Queen
    if ((originPieceIsWhitePawn && dest.row() == 8) || (originPieceIsBlackPawn && dest.row() == 1)) {
        an.push_back('=');
        an.push_back('Q');
    }

    // if the next state results in opponent's king being checked, note that
    MateStatus ms = isCheckmate(nextState.value(), ~_mv.colorOfAppliedPiece());
    if (ms == MateStatus::CHECK) {
        an.push_back('x');
    } else if (ms == MateStatus::CHECKMATE) {
        an.push_back('x');
        an.push_back('x');
    }

    return { an, nextState.value() };
}
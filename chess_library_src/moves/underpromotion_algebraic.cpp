#include "underpromotion_algebraic.h"
#include "../check_terminal/checkmate.h"

#include <stdexcept>

UnderpromotionAlgebraic::UnderpromotionAlgebraic(Underpromotion mv, std::string resolveAmbiguity) noexcept :
    _mv(mv), _resolveAmbiguity(resolveAmbiguity) {}

MoveResult UnderpromotionAlgebraic::performMove(const ChessBoard& state) {

    using namespace CheckTerminal;

    // verify whether the move is valid for the given state
    std::optional<ChessBoard> nextState = _mv(state);
    if (!nextState.has_value()) {
        throw std::invalid_argument("The underpromotion move is not valid for the given state");
    }
    
    std::string an;

    // resolve ambiguity
    an += _resolveAmbiguity;

    // confirm if the piece captures something
    Coord2D dest = _mv.origin() + _mv.moveVec();
    if (state.getPiece(dest) != static_cast<Piece_t>(ChessPiece::NONE)) {
        an.push_back('x');
    }

    // get destination square
    an.push_back(dest.col());
    an.push_back(dest.row() + '0');
    an.push_back('=');

    // pawn promotion to piece: 
    switch (_mv.promotePiece()) {
        case ChessPiece::WHITE_ROOK:
        case ChessPiece::BLACK_ROOK:
            an.push_back('R');
            break;

        case ChessPiece::WHITE_KNIGHT:
        case ChessPiece::BLACK_KNIGHT:
            an.push_back('N');
            break;

        default:
            an.push_back('B');
            break;
    }

    // if the resulting state checks the opponent's king, note that
    MateStatus ms = isCheckmate(nextState.value(), ~_mv.colorOfAppliedPiece());
    if (ms == MateStatus::CHECK) {
        an.push_back('x');
    } else if (ms == MateStatus::CHECKMATE) {
        an.push_back('x');
        an.push_back('x');
    }

    return { an, nextState.value() };
}
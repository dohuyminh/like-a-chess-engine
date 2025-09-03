#include "underpromotion_algebraic.h"
#include "../check_terminal/checkmate.h"

#include <stdexcept>

namespace internal {

UnderpromotionAlgebraic::UnderpromotionAlgebraic(Underpromotion mv) noexcept :
    _mv(mv)
{}

MoveResult UnderpromotionAlgebraic::performMove(const ChessBoard& board) {

    using namespace CheckTerminal;

    // verify whether the move is valid for the given board
    std::optional<ChessBoard> nextboard = _mv(board);
    if (!nextboard.has_value()) {
        throw std::invalid_argument("The underpromotion move is not valid for the given board");
    }
    
    // underpromotion always involves a pawn move
    // so we can directly note that a pawn has moved
    bool capture = false;

    std::string an;

    // resolve ambiguity
    an += _mv.origin();

    // confirm if the piece captures something
    Coord2D dest = _mv.origin() + _mv.moveVec();
    if (!board.getPiece(dest).isNone()) {
        capture = true;
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

    // if the resulting board checks the opponent's king, note that
    MateStatus ms = isCheckmate(nextboard.value(), ~_mv.colorOfAppliedPiece());
    if (ms == MateStatus::CHECK) {
        an.push_back('+');
    } else if (ms == MateStatus::CHECKMATE) {
        an.push_back('#');
    }

    return { an, nextboard.value(), capture, true };
}

}
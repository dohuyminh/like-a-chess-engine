#include "checkmate.h"
#include "check.h"

namespace internal {
    
namespace CheckTerminal {
    MateStatus isCheckmate(const ChessBoard& board, Color kingsColor, std::size_t numLegalMoves) {
        // if the king is not checked, it cannot be checkmate
        std::unordered_set<Coord2D> checkingPieces = kingIsChecked(board, kingsColor);
        if (checkingPieces.empty()) {
            return MateStatus::NONE;
        }

        return numLegalMoves ? MateStatus::CHECK : MateStatus::CHECKMATE;
    }
}

}

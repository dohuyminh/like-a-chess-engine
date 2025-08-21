#include "checkmate.h"
#include "check.h"
#include "../moves/get_all_moves.h"
#include "../moves/queens_move.h"
#include "../moves/knights_move.h"
#include "../moves/underpromotion.h"

#include <stdexcept>

namespace CheckTerminal {
    MateStatus isCheckmate(const ChessBoard& board, bool kingIsWhite) {
        // if the king is not checked, it cannot be checkmate
        std::unordered_set<Coord2D> checkingPieces = kingIsChecked(board, kingIsWhite);
        if (checkingPieces.empty()) {
            return MateStatus::NONE;
        }

        std::vector<std::shared_ptr<ChessMove>> allMoves = getAllMoves(board, kingIsWhite);
        if (!allMoves.empty()) {
            return MateStatus::CHECK;
        }
        return MateStatus::CHECKMATE;
    }
}
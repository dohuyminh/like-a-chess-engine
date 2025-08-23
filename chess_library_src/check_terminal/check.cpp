#include "check.h"
#include "../square_iterator/next_square_iterator.h"

#include <stdexcept>

namespace CheckTerminal
{
    std::unordered_set<Coord2D> kingIsChecked(const ChessBoard& board, Color kingsColor) {
        
        // store all coordinates of pieces capturing the king 
        std::unordered_set<Coord2D> res;

        // find the king's position
        Coord2D kingCoord = kingsColor == Color::WHITE ? board.whiteKingCoord() : board.blackKingCoord();
        ChessPiece kingPiece = board.getPiece(kingCoord);

        // scan the board for all pieces of opposite color 
        for (char col = 'a'; col <= 'h'; ++col) {
            for (int8_t row = 1; row <= 8; ++row) {
                
                Coord2D coord(col, row);
                
                // if not the color -> skip
                ChessPiece currPiece = board.getPiece(coord);
                if (!currPiece.captures(kingPiece)) {
                    continue;
                }
                
                std::vector<Coord2D> canReach = findAvailableSquares(board, coord);
                if (std::find(canReach.begin(), canReach.end(), kingCoord) != canReach.end()) {
                    res.insert(coord);
                }
            }
        }

        // no piece is checking the king; return false
        return res;
    
    }

} // namespace CheckTerminal

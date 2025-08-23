#include "r_rook.h"

std::vector<Coord2D> RRook::pieceCanReachSquare(const ChessBoard& state, Coord2D origin) const {

    std::vector<Coord2D> res;
    
    ChessPiece pieceAtOrigin = state.getPiece(origin);
    if (pieceAtOrigin.isNone()) {
        return res;
    }

    // only scan for up; down; left; right 
    const Vec2D mv[4] = { Vec2D(0, 1), Vec2D(0, -1), Vec2D(-1, 0), Vec2D(1, 0) };
    bool canGo[4];
    std::fill(canGo, canGo + 4, true);
    int nSteps = 1; 

    // iterate each step
    do {

        for (int8_t i = 0; i < 4; ++i) {
            
            char nextCol; int8_t nextRow;

            if (canGo[i]) {
                // see if the position is out of bounds
                nextCol = origin.col() + mv[i].mvCol() * nSteps;
                nextRow = origin.row() + mv[i].mvRow() * nSteps;
    
                canGo[i] = ('a' <= nextCol && nextCol <= 'h') && (1 <= nextRow && nextRow <= 8);
            }

            if (canGo[i]) {
                // verify if the position is not occupied by same-color piece 
                Coord2D dest(nextCol, nextRow);
                ChessPiece pieceAtDest = state.getPiece(dest); 
                bool isNone = pieceAtDest.isNone();
                bool isCapture = pieceAtOrigin.captures(pieceAtDest);

                if (!isNone && !isCapture) {
                    canGo[i] = false; continue;
                }

                res.push_back(dest);

                // if it's a capture, stop there
                if (isCapture) {
                    canGo[i] = false;
                }   
            }
        }

        // increase the magnitude 
        ++nSteps;

    } while (canGo[0] || canGo[1] || canGo[2] || canGo[3]);

    return res;

};
#pragma once

#include "../chess_board.h"

/**
 * @brief Base class for all chess moves.
 * This class defines the interface for a chess move, which can be applied to a ChessBoard state.
 * For a specific chess move object, one can call the operator() to apply the move to a given ChessBoard state.
 * If the move is valid, it returns a new ChessBoard state; otherwise, it returns ```std::nullopt```.
 */
class ChessMove {
protected:

    static void updateCastling(
        const ChessBoard& state, 
        bool originPieceIsWhite,
        Coord2D newPoint, 
        bool& newWhiteLeftCastling, 
        bool& newWhiteRightCastling, 
        bool& newBlackLeftCastling, 
        bool& newBlackRightCastling);
            
        bool _isWhite;
        
public:
    ChessMove(bool appliedPieceIsWhite);

    inline bool whiteMakesMove() {
        return _isWhite;
    }

    virtual std::optional<ChessBoard> operator()(const ChessBoard& state) const = 0;
    virtual ~ChessMove() = default;
};
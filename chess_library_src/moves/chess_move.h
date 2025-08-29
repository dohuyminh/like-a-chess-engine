#pragma once

#include "../chess_board.h"

namespace internal
{
    
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
        Color turn,
        Coord2D newPoint, 
        char* boardData);
            
    Color _color;
        
public:
    constexpr explicit ChessMove(Color colorOfAppliedPiece) : _color(colorOfAppliedPiece) {
        if (_color == Color::NONE) {
            throw std::invalid_argument("Pieces can only be moved if the color is White/Black");
        }
    }

    [[nodiscard]] inline Color colorOfAppliedPiece() const {
        return _color;
    }

    virtual std::optional<ChessBoard> operator()(const ChessBoard& state) const = 0;
    virtual ~ChessMove() = default;
};
    
} // namespace internal

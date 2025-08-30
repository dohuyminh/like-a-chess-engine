#pragma once

#include "chess_move.h"

namespace internal {

/**
 * @brief A class representing an underpromotion move in chess.
 * An underpromotion allows a pawn to be promoted to a piece other than a queen (i.e., rook, bishop, knight) when it reaches the last row.
 * This transformation can be applied to either a white or black pawn, and makes pedantic checks to ensure the move is only applied 
 * to a pawn at the second last row (i.e., row 7 for white, row 2 for black). Should the check fail, it throws an ```std::invalid_argument``` exception.
 */
class Underpromotion final : public ChessMove {
public:
    Underpromotion(Color color, Direction direction, Coord2D origin, ChessPiece promotePiece);

    std::optional<ChessBoard> operator()(const ChessBoard& board) const override;

    [[nodiscard]] inline Coord2D origin() const {
        return _origin;
    }

    [[nodiscard]] inline Vec2D moveVec() const {
        return vecMap[_direction] * (_color == Color::WHITE ? 1 : -1);
    }

    [[nodiscard]] inline ChessPiece promotePiece() const {
        return _promotePiece;
    }

    ~Underpromotion() override = default;

private:
    Direction _direction;
    Coord2D _origin;
    ChessPiece _promotePiece;
};

}
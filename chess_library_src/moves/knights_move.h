#pragma once

#include "chess_move.h"

namespace internal {

/**
 * @brief A class representing a Knight's move in chess.
 * A Knight's move allows a piece to move in an "L" shape: two squares in one direction and then one square perpendicular, or vice versa.
 * This transformation can be applied to either a white or black knight, and makes pedantic checks to ensure the move does not result in an out-of-bounds coordinate.
 * If the check fails, it throws an ```std::invalid_argument``` exception.
 */
class KnightsMove final : public ChessMove {
public:
    KnightsMove(Color color, Vec2D direction, Coord2D origin);

    std::optional<ChessBoard> operator()(const ChessBoard& board) const override;

    [[nodiscard]] inline Coord2D origin() const {
        return _origin;
    } 

    [[nodiscard]] inline Vec2D moveVec() const {
        return _direction;
    }

    ~KnightsMove() override = default;

private:
    Vec2D _direction;
    Coord2D _origin;
};

}
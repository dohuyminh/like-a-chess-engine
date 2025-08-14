#pragma once

#include "chess_move.h"


/**
 * @brief A class representing a Queen's move in chess.
 * A Queen's move allows a piece to move either horizontally, vertically, or diagonally any number of squares.
 * This transformation can be applied to either a white or black pawn/rook/bishop/queen. The transformation also adheres to the moveset of
 * each piece type (e.g. rook cannot move diagonally).
 * 
 * The constructor of the class makes pedantic check to ensure the move does not result in an out-of-bounds coordinate. If the check fails,
 * it throws an ```std::invalid_argument``` exception.
 * 
 * NOTE: A pawn's promotion to a queen piece is the default beehavior once it reaches the last row.
 * If the pawn is promoted to a different piece, use the Underpromotion class instead.
 */
class QueensMove final : public ChessMove {
public:
    QueensMove(bool appliedPieceIsWhite, Direction direction, uint8_t numSteps, Coord2D origin); 
    
    std::optional<ChessBoard> operator()(const ChessBoard& state) const override;

    inline Coord2D origin() const {
        return _origin;
    }

    inline Vec2D moveVec() const {
        return vecMap[_direction] * _numSteps * (_isWhite ? 1 : -1);
    }

    ~QueensMove() override = default;

private:
    Direction _direction;
    uint8_t _numSteps;
    Coord2D _origin; 
};
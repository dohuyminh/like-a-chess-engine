#pragma once

#include "chess_move.h"

/**
 * @brief A class representing a castling move in chess.
 * Castling is a special move that allows the king and a rook to move simultaneously if the king is not in check, 
 * the squares between them are unoccupied, and neither piece has moved before. No pedantic checks are performed in the constructor,
 * but if the transformation is performed on a game state that does not allow castling, or there are pieces between 
 * the rook and the king, it returns ```std::nullopt```.  
 */
class Castling final : public ChessMove {
public:
    Castling(bool isWhiteTurn, bool isLeft) noexcept;

    inline bool moveLeft() const noexcept {
        return _isLeft;
    }

    std::optional<ChessBoard> operator()(const ChessBoard& state) const override;

    ~Castling() override = default;

private:
    bool _isLeft;
};


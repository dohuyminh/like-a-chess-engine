#pragma once

#include "chess_board.h"
#include <cstdint>

class ChessMove {
protected:

    explicit ChessMove(bool appliedPieceIsWhite);

    static void updateCastling(
        const ChessBoard& state, 
        bool originPieceIsWhite,
        Coord2D newPoint, 
        bool& newWhiteLeftCastling, 
        bool& newWhiteRightCastling, 
        bool& newBlackLeftCastling, 
        bool& newBlackRightCastling);
    
    virtual std::optional<ChessBoard> operator()(const ChessBoard& state) const = 0;

    bool _isWhite;

public:
    virtual ~ChessMove() = default;
};

class QueensMove final : public ChessMove {
public:
    QueensMove(bool appliedPieceIsWhite, Direction direction, uint8_t numSteps, Coord2D origin); 
    
    std::optional<ChessBoard> operator()(const ChessBoard& state) const override;

    ~QueensMove() override = default;

private:
    Direction _direction;
    uint8_t _numSteps;
    Coord2D _origin; 
};

class KnightsMove final : public ChessMove {
public:
    KnightsMove(bool appliedPieceIsWhite, Vec2D direction, Coord2D origin);

    std::optional<ChessBoard> operator()(const ChessBoard& state) const override;

    ~KnightsMove() override = default;

private:
    Vec2D _direction;
    Coord2D _origin;
};

class Underpromotion final : public ChessMove {
public:
    Underpromotion(bool appliedPieceIsWhite, Direction direction, Coord2D origin, ChessPiece promotePiece);

    std::optional<ChessBoard> operator()(const ChessBoard& state) const override;

    ~Underpromotion() override = default;

private:
    Direction _direction;
    Coord2D _origin;
    ChessPiece _promotePiece;
};

class Castling final : public ChessMove {
public:
    Castling(bool isWhiteTurn, bool isLeft);

    std::optional<ChessBoard> operator()(const ChessBoard& state) const override;

    ~Castling() override = default;

private:
    bool _isLeft;
};

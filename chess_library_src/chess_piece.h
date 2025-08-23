#pragma once

typedef char Piece_t;

#include <stdexcept>

class Color {
public:
    enum _Color : char {
        NONE, WHITE, BLACK  
    };

    constexpr Color(_Color color) : _color(color) {}

    constexpr operator _Color() const noexcept {
        return _color;
    }

    inline Color operator~() const {
        if (_color == NONE) {
            throw std::logic_error("Color switching may only be applied for color white/black");
        }
        return _color == WHITE ? BLACK : WHITE; 
    } 

private:
    
    _Color _color;
};

class ChessPiece {
public:
    enum Piece : char {
        NONE = 1,
        WHITE_PAWN, 
        WHITE_ROOK, 
        WHITE_KNIGHT, 
        WHITE_BISHOP, 
        WHITE_QUEEN, 
        WHITE_KING,
        BLACK_PAWN, 
        BLACK_ROOK, 
        BLACK_KNIGHT, 
        BLACK_BISHOP, 
        BLACK_QUEEN, 
        BLACK_KING
    };

    constexpr ChessPiece(Piece piece) : _piece(piece) {}
    constexpr explicit ChessPiece(Piece_t piece) : _piece(static_cast<Piece>(piece)) {}

    constexpr operator Piece() const {
        return _piece;
    }

    inline const char* toAscii() const {
        return PieceToAscii[_piece - 1];
    }

    inline bool isNone() const {
        return _piece == NONE;
    }

    inline bool isWhite() const {
        return WHITE_PAWN <= _piece && _piece <= WHITE_KING;
    }

    inline bool isBlack() const {
        return BLACK_PAWN <= _piece && _piece <= BLACK_KING;
    }

    inline Color color() const {
        if (isNone()) return Color::NONE;
        if (isWhite()) return Color::WHITE;
        return Color::BLACK;
    } 

    inline bool captures(ChessPiece other) const {
        return !isNone() && !other.isNone() && ((isWhite() && other.isBlack()) || (isBlack() && other.isWhite()));
    }

    inline bool isPawn() const {
        return _piece == WHITE_PAWN || _piece == BLACK_PAWN;
    }

    inline bool isRook() const {
        return _piece == WHITE_ROOK || _piece == BLACK_ROOK;
    }

    inline bool isKnight() const {
        return _piece == WHITE_KNIGHT || _piece == BLACK_KNIGHT;
    }

    inline bool isBishop() const {
        return _piece == WHITE_BISHOP || _piece == BLACK_BISHOP;
    }

    inline bool isQueen() const {
        return _piece == WHITE_QUEEN || _piece == BLACK_QUEEN;
    }

    inline bool isKing() const {
        return _piece == WHITE_KING || _piece == BLACK_KING;
    }

private:

    static constexpr char PieceToAscii[13][4] = {
        " ", // NONE
        "♙", // WHITE_PAWN
        "♖", // WHITE_ROOK
        "♘", // WHITE_KNIGHT
        "♗", // WHITE_BISHOP
        "♕", // WHITE_QUEEN
        "♔", // WHITE_KING
        "♟", // BLACK_PAWN
        "♜", // BLACK_ROOK
        "♞", // BLACK_KNIGHT
        "♝", // BLACK_BISHOP
        "♛", // BLACK_QUEEN
        "♚"  // BLACK_KING
    };

    Piece _piece;
};

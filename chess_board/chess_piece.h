#pragma once

typedef char Piece_t;

enum class ChessPiece : char {
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

inline bool pieceIsWhite(Piece_t piece) {
    return static_cast<char>(ChessPiece::WHITE_PAWN) <= piece &&
            piece <= static_cast<char>(ChessPiece::WHITE_KING);
}

inline bool pieceIsBlack(Piece_t piece) {
    return static_cast<char>(ChessPiece::BLACK_PAWN) <= piece &&
            piece <= static_cast<char>(ChessPiece::BLACK_KING);
}

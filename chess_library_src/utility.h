#pragma once

#include "chess_piece.h"
#include "coord2D.h"

#include <optional>
#include <stdexcept>

namespace internal {

namespace utility { 

inline void writeData(char* boardData, Coord2D coord, ChessPiece piece) {
    
    // get flat index, then locate the byte containing the coordinate
    uint8_t flatIdx = coord.toFlatIdx(), byteLocn = flatIdx >> 1;

    // create the bit mask
    uint8_t mask = (1 << 4) - 1;
    Piece_t rawPiece = piece;
    if ((coord.col() - 'a') & 1) {
        mask <<= 4;
        rawPiece <<= 4;
    }

    // clear out original data 
    boardData[byteLocn] &= ~mask;

    // insert new data 
    boardData[byteLocn] |= rawPiece;
}

[[nodiscard]] inline ChessPiece getData(const char* boardData, Coord2D coord) {
    // get flat index, then locate the byte containing the coordinate
    uint8_t flatIdx = coord.toFlatIdx(), byteLocn = flatIdx >> 1;
    char byte = boardData[byteLocn];

    // create bitmask
    uint8_t mask = (1 << 4) - 1;

    // if the columns are b/d/f/h -> bit shift appropriately
    if ((coord.col() - 'a') & 1) {
        byte >>= 4;
    }


    return ChessPiece(byte & mask);
}

[[nodiscard]] inline bool getCastling(const char* boardData, Color color, bool isQueenSide) {
    
    if (color == Color::NONE) {
        throw std::invalid_argument("Getting castling rights of an undefined player");
    }

    // get the bit position corresponding to the color and the side 
    int location = 2 * (color == Color::BLACK) + !isQueenSide;

    // create the bitmask
    char mask = 1 << location;

    // get the result
    return boardData[32] & mask;
}

inline void turnOffCastling(char* boardData, Color color, bool isQueenSide) {
    
    if (color == Color::NONE) {
        throw std::invalid_argument("Setting castling rights of an undefined player");
    }

    // get the bit position corresponding to color and the side
    int location = 2 * (color == Color::BLACK) + !isQueenSide;

    // create bit mask 
    char mask = ~(1 << location);

    // flip the result
    boardData[32] &= mask;
}

[[nodiscard]] inline std::optional<Coord2D> getEnPassant(const char* boardData, Color color) {

    if (color == Color::NONE) {
        throw std::invalid_argument("Getting en passant square of an undefined player");
    }

    // check if en passant is allowed in the first place
    // en passant data is in byte 34
    char enPassantData = boardData[33];

    // 8th bit: whether any square is en passant square
    // 7th bit: whether it's a white/black en passant
    bool exist   = (enPassantData & (1 << 7)) >> 7;
    bool isWhite = (enPassantData & (1 << 6)) >> 6;

    if (!exist || isWhite != (color == Color::WHITE)) return std::nullopt;

    // extract 1st-3rd bit for column, 4th-6th bit for row;
    uint8_t mask = (1 << 3) - 1;
    uint8_t colIdx =  enPassantData       & mask, col = colIdx + 'a';
    uint8_t rowIdx = (enPassantData >> 3) & mask;
    int8_t row  = rowIdx + 1;

    // transform index to coordinates
    return std::optional<Coord2D>(Coord2D(col, row));

}

inline void setEnPassant(char* boardData, Color color, Coord2D square) {
    
    if (color == Color::NONE) {
        throw std::invalid_argument("Getting en passant square of an undefined player");
    }

    // en passant data is in byte 34
    // enable en passant toggle 
    boardData[33] |= 1 << 7;

    // enable color (1: white, 0: black)
    char playerMask = (color == Color::WHITE) ? 1 << 6 : 0;
    boardData[33] |= playerMask;

    // write coordinate into first 6 bits
    char colEncoding = square.col() - 'a', colMask = colEncoding;
    char rowEncoding = square.row() - 1,   rowMask = rowEncoding << 3;

    // clear out the original data 
    char clearMask = ~((1 << 6) - 1);
    boardData[33] &= clearMask;

    // insert new data
    boardData[33] |= colMask;
    boardData[33] |= rowMask;
}

inline void turnOffEnpassant(char* boardData) {
    boardData[33] = 0;
}

}

}
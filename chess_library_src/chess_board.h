#pragma once

#include "coord2D.h"
#include "chess_piece.h"
#include "utility.h"

#include <cstddef>
#include <cstring>
#include <functional>
#include <string>
#include <optional>
#include <unordered_set>
#include <vector>

#include <iostream>

/**
 * @brief A representation of the current chess board state. 
 * 
 * This class encodes the board state; this includes the position of each piece, en passant square, castling rights
 */
class ChessBoard {
    
public:
    
    ChessBoard();

    ChessBoard(const char* boardData);

    [[nodiscard]] inline const char* boardData() const {
        return _boardData;
    }
    
    [[nodiscard]] inline bool castling(Color color, bool isQueenside) const {
        return internal::utility::getCastling(_boardData, color, isQueenside);
    } 

    [[nodiscard]] inline std::optional<Coord2D> enpassant(Color color) const {
        return internal::utility::getEnPassant(_boardData, color);
    }

    [[nodiscard]] inline ChessPiece getPiece(Coord2D coord) const {
        return internal::utility::getData(_boardData, coord);
    }

    inline bool operator==(const ChessBoard& other) const {
        return memcmp(_boardData, other.boardData(), 34) == 0;
    }

    [[nodiscard]] std::string getWhitePOV() const;
    [[nodiscard]] std::string getBlackPOV() const;
    
private:
    
    void _initData();

    // raw board representation
    char _boardData[34] = { 0 };

};

template<>
struct std::hash<ChessBoard> {

    std::size_t operator()(const ChessBoard& board) const noexcept {
        std::hash<std::string> rawBoardHasher;
        std::size_t hRawBoard = rawBoardHasher(std::string(board.boardData(), board.boardData() + 34));
        return hRawBoard;
    }

};

#pragma once

#include "coord2D.h"
#include "chess_piece.h"

#include <cstddef>
#include <functional>
#include <string>
#include <optional>
#include <unordered_set>
#include <vector>

/**
 * @brief A representation of the current chess board state. 
 * 
 * This class encodes the board state; this includes the position of each piece, en passant square, castling rights
 */
class ChessBoard {
    
public:
    
    ChessBoard();

    ChessBoard(
        std::string  board,
        Coord2D whiteKingCoord,
        Coord2D blackKingCoord,
        bool whiteLeftCastling,
        bool whiteRightCastling,
        bool blackLeftCastling,
        bool blackRightCastling,
        const std::optional<Coord2D>& whiteEnpassant, 
        const std::optional<Coord2D>& blackEnpassant
    );

    inline const std::string& board() const {
        return _board;
    }
    
    inline bool whiteLeftCastling() const {
        return _whiteLeftCastling;
    }

     inline bool blackLeftCastling() const {
        return _blackLeftCastling;
    }

     inline bool whiteRightCastling() const {
        return _whiteRightCastling;
    }

     inline bool blackRightCastling() const {
        return _blackRightCastling;
    }

     inline const std::optional<Coord2D>& whiteEnpassant() const {
        return _whiteEnpassant;
    }

     inline const std::optional<Coord2D>& blackEnpassant() const {
        return _blackEnpassant;
    }

    inline const Coord2D& whiteKingCoord() const {
        return _whiteKingCoord;
    }

     inline const Coord2D& blackKingCoord() const {
        return _blackKingCoord;
    }

    ChessPiece getPiece(Coord2D coord) const;
    bool operator==(const ChessBoard& other) const;
    std::string getWhitePOV() const;
    std::string getBlackPOV() const;
    
private:
    
    static std::string initRawBoard();
    static constexpr int8_t BOARD_SIZE = 8;

    // raw board representation
    std::string _board;

    // specify the king's coordinates
    Coord2D _whiteKingCoord;
    Coord2D _blackKingCoord;

    // specify whether the white player can perform castling
    bool _whiteLeftCastling;
    bool _whiteRightCastling;

    // specify whether the black player can perform castling
    bool _blackLeftCastling;
    bool _blackRightCastling;

    // specify which pawn just performed a 2-square advance; said
    // pawn is susceptible to en passant 
    std::optional<Coord2D> _whiteEnpassant;
    std::optional<Coord2D> _blackEnpassant;

};

template<>
struct std::hash<ChessBoard> {

    std::size_t operator()(const ChessBoard& board) const noexcept {
        std::hash<std::string> rawBoardHasher;
        std::size_t hRawBoard = rawBoardHasher(board.board());

        hRawBoard <<=     (board.whiteLeftCastling());
        hRawBoard <<= 2 * (board.blackLeftCastling());
        hRawBoard <<= 3 * (board.whiteEnpassant().has_value());
        hRawBoard <<= 4 * (board.blackEnpassant().has_value());
        hRawBoard <<= 5 * (board.whiteRightCastling());
        hRawBoard <<= 6 * (board.blackRightCastling());

        return hRawBoard;
    }

};

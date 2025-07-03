#pragma once

#include "chess_piece.h"

#include <cstddef>
#include <functional>
#include <string>
#include <optional>
#include <unordered_map>
#include <utility>

typedef std::pair<int, char> Coord2D;

class ChessBoard {
    
public:
    
    ChessBoard();

    ChessBoard(
        const std::string& board, 
        bool whiteLeftRookMoved, 
        bool whiteRightRookMoved,
        bool whiteKingMoved,
        bool blackLeftRookMoved, 
        bool blackRightRookMoved,
        bool blackKingMoved,
        const std::optional<Coord2D>& whiteEnpassant, 
        const std::optional<Coord2D>& blackEnpassant
    );

    inline std::string board() const {
        return _board;
    }
    
    inline bool whiteLeftCastling() const {
        return !(_whiteKingMoved || _whiteLeftRookMoved);
    }

    inline bool blackLeftCastling() const {
        return !(_blackKingMoved || _blackLeftRookMoved);
    }

    inline bool whiteRightCastling() const {
        return !(_whiteKingMoved || _whiteRightRookMoved);
    }

    inline bool blackRightCastling() const {
        return !(_whiteKingMoved || _blackRightRookMoved);
    }

    inline const std::optional<Coord2D>& whiteEnpassant() const {
        return _whiteEnpassant;
    }

    inline const std::optional<Coord2D>& blackEnpassant() const {
        return _blackEnpassant;
    }

    Piece_t getPiece(Coord2D coord);
    bool operator==(const ChessBoard& other);
    operator std::string();

private:
    
    static std::string initRawBoard();
    static constexpr int BOARD_SIZE = 8;

    // raw board representation
    std::string _board;

    // specify whether the white player can perform castling
    bool _whiteLeftRookMoved;
    bool _whiteRightRookMoved;
    bool _whiteKingMoved;

    // specify whether the black player can perform castling
    bool _blackLeftRookMoved;
    bool _blackRightRookMoved;
    bool _blackKingMoved;

    // specify which pawn just performed a 2-square advance; said
    // pawn is susceptible to en passant 
    std::optional<Coord2D> _whiteEnpassant;
    std::optional<Coord2D> _blackEnpassant;

};

template<>
struct std::hash<ChessBoard> {

    std::size_t operator()(const ChessBoard& board) {
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

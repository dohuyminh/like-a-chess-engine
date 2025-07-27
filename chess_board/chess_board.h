#pragma once

#include "coord2D.h"
#include "chess_piece.h"

#include <cstddef>
#include <functional>
#include <string>
#include <optional>
#include <unordered_set>
#include <vector>


class ChessBoard {
    
public:
    
    ChessBoard();

    ChessBoard(
        std::string  board,
        bool whiteLeftCastling,
        bool whiteRightCastling,
        bool blackLeftCastling,
        bool blackRightCastling,
        const std::optional<Coord2D>& whiteEnpassant, 
        const std::optional<Coord2D>& blackEnpassant
    );

    [[nodiscard]] inline const std::string& board() const {
        return _board;
    }
    
    [[nodiscard]] inline bool whiteLeftCastling() const {
        return _whiteLeftCastling;
    }

    [[nodiscard]] inline bool blackLeftCastling() const {
        return _blackLeftCastling;
    }

    [[nodiscard]] inline bool whiteRightCastling() const {
        return _whiteRightCastling;
    }

    [[nodiscard]] inline bool blackRightCastling() const {
        return _whiteRightCastling;
    }

    [[nodiscard]] inline const std::optional<Coord2D>& whiteEnpassant() const {
        return _whiteEnpassant;
    }

    [[nodiscard]] inline const std::optional<Coord2D>& blackEnpassant() const {
        return _blackEnpassant;
    }

    std::unordered_set<Coord2D> kingIsChecked(bool kingIsWhite);
    
    Piece_t getPiece(Coord2D coord) const;
    bool operator==(const ChessBoard& other) const;
    std::string getWhitePOV();
    std::string getBlackPOV();
    static std::string initRawBoard();

private:
    
    bool pieceCanCaptureKing(Coord2D pieceCoord);

    static constexpr int8_t BOARD_SIZE = 8;

    // raw board representation
    const std::string _board;

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

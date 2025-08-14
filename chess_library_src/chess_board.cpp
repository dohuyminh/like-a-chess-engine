#include "chess_board.h"
#include <stdexcept>
#include <utility>

// #include <pybind11/pybind11.h>

// namespace py = pybind11;

/**
 * @brief A private static method of `ChessBoard` class. Returns a string representation of a chess board
 * when starting the game.
 * 
 * @return `std::string` raw string as initial board state  
 */
std::string ChessBoard::initRawBoard() {
    std::string rawBoard(
        ChessBoard::BOARD_SIZE * ChessBoard::BOARD_SIZE, 
        static_cast<char>(ChessPiece::NONE)
    );
    
    // assign pawns
    for (int p =  8; p <= 15; ++p) rawBoard[p] = static_cast<char>(ChessPiece::WHITE_PAWN);
    for (int p = 48; p <= 55; ++p) rawBoard[p] = static_cast<char>(ChessPiece::BLACK_PAWN);

    // assign everything else 
    rawBoard[0] = rawBoard[7] = static_cast<char>(ChessPiece::WHITE_ROOK);
    rawBoard[1] = rawBoard[6] = static_cast<char>(ChessPiece::WHITE_KNIGHT);
    rawBoard[2] = rawBoard[5] = static_cast<char>(ChessPiece::WHITE_BISHOP);
    rawBoard[4] = static_cast<char>(ChessPiece::WHITE_KING);
    rawBoard[3] = static_cast<char>(ChessPiece::WHITE_QUEEN);

    rawBoard[56] = rawBoard[63] = static_cast<char>(ChessPiece::BLACK_ROOK);
    rawBoard[57] = rawBoard[62] = static_cast<char>(ChessPiece::BLACK_KNIGHT);
    rawBoard[58] = rawBoard[61] = static_cast<char>(ChessPiece::BLACK_BISHOP);
    rawBoard[60] = static_cast<char>(ChessPiece::BLACK_KING);
    rawBoard[59] = static_cast<char>(ChessPiece::BLACK_QUEEN);

    return rawBoard;
}

ChessBoard::ChessBoard() : 
    _board(ChessBoard::initRawBoard()), 
    _whiteLeftCastling(true),
    _whiteRightCastling(true),
    _blackLeftCastling(true),
    _blackRightCastling(true),
    _whiteEnpassant(std::nullopt),
    _blackEnpassant(std::nullopt),
    _whiteKingCoord('E', 1),
    _blackKingCoord('E', 8) {

}

ChessBoard::ChessBoard(
        std::string board,
        Coord2D whiteKingCoord,
        Coord2D blackKingCoord,
        const bool whiteLeftCastling,
        const bool whiteRightCastling,
        const bool blackLeftCastling,
        const bool blackRightCastling,
        const std::optional<Coord2D>& whiteEnpassant,
        const std::optional<Coord2D>& blackEnpassant
    ) : 
    _board(std::move(board)),
    _whiteLeftCastling(whiteLeftCastling),
    _whiteRightCastling(whiteRightCastling),
    _blackLeftCastling(blackLeftCastling),
    _blackRightCastling(blackRightCastling),
    _whiteEnpassant(whiteEnpassant),
    _blackEnpassant(blackEnpassant),
    _whiteKingCoord(std::move(whiteKingCoord)),
    _blackKingCoord(std::move(blackKingCoord)) {

}

Piece_t ChessBoard::getPiece(Coord2D coord) const {
    
    if (!(coord.row() >= 1 && coord.row() <= 8) && (coord.col() >= 'A' && coord.col() <= 'H')) {
        throw std::out_of_range("Position on the board must be <1-8, A-H>");
    }

    int idx = coord.toFlatIdx(); 
    return _board[idx];
}

bool ChessBoard::operator==(const ChessBoard& other) const {
    if (this->_board != other._board) {
        return false;
    }

    if (this->_whiteLeftCastling != other._whiteLeftCastling || this->_blackLeftCastling != other._blackLeftCastling) {
        return false;
    }

    if (this->_whiteRightCastling != other._whiteRightCastling || this->_blackRightCastling != other._blackRightCastling) {
        return false;
    }

    if (this->_whiteEnpassant != other._whiteEnpassant || this->_blackEnpassant != other._blackEnpassant) {
        return false;
    }

    return true;
}

std::string ChessBoard::getWhitePOV() const {
    std::string rep{"  a b c d e f g h\n"};
    
    for (int8_t row = BOARD_SIZE - 1; row >= 0; --row) {
        
        rep.push_back(static_cast<char>('1' + row));
        rep.push_back('|');

        for (int8_t col = 0; col < BOARD_SIZE; ++col) {

            const int8_t idx = row * BOARD_SIZE + col;

            Piece_t piece = _board[idx];
            rep.append(PieceToAscii[piece - 1]); 
            rep.push_back('|');
        } 
        
        rep.push_back(static_cast<char>('1' + row));
        rep.push_back('\n');
    } 

    rep.append("  a b c d e f g h\n");

    return rep;
}

std::string ChessBoard::getBlackPOV() const {
    std::string rep{"  h g f e d c b a\n"};

    for (int8_t row = 0; row < BOARD_SIZE; ++row) {

        rep.push_back(static_cast<char>('1' + row));
        rep.push_back('|');

        for (int8_t col = BOARD_SIZE - 1; col >= 0; --col) {

            const int8_t idx = row * BOARD_SIZE + col;

            Piece_t piece = _board[idx];
            rep.append(PieceToAscii[piece - 1]); 
            rep.push_back('|');
        }

        rep.push_back(static_cast<char>('1' + row));
        rep.push_back('\n');
    }

    rep.append("  h g f e d c b a\n");

    return rep;
}

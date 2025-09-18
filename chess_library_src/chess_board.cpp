#include "chess_board.h"
#include "chess_piece.h"
#include "utility.h"

// #include <pybind11/pybind11.h>

// namespace py = pybind11;

void ChessBoard::_initData() {
    
    // initialize pawns 
    for (char col = 'a'; col <= 'h'; ++col) {
        internal::utility::writeData(_boardData, Coord2D(col, 2), ChessPiece::WHITE_PAWN);
        internal::utility::writeData(_boardData, Coord2D(col, 7), ChessPiece::BLACK_PAWN);
    }

    // initialize rooks
    internal::utility::writeData(_boardData, Coord2D('a', 1), ChessPiece::WHITE_ROOK);
    internal::utility::writeData(_boardData, Coord2D('h', 1), ChessPiece::WHITE_ROOK);
    internal::utility::writeData(_boardData, Coord2D('a', 8), ChessPiece::BLACK_ROOK);
    internal::utility::writeData(_boardData, Coord2D('h', 8), ChessPiece::BLACK_ROOK);

    // initialize knights
    internal::utility::writeData(_boardData, Coord2D('b', 1), ChessPiece::WHITE_KNIGHT);
    internal::utility::writeData(_boardData, Coord2D('g', 1), ChessPiece::WHITE_KNIGHT);
    internal::utility::writeData(_boardData, Coord2D('b', 8), ChessPiece::BLACK_KNIGHT);
    internal::utility::writeData(_boardData, Coord2D('g', 8), ChessPiece::BLACK_KNIGHT);

    // initialize bishops
    internal::utility::writeData(_boardData, Coord2D('c', 1), ChessPiece::WHITE_BISHOP);
    internal::utility::writeData(_boardData, Coord2D('f', 1), ChessPiece::WHITE_BISHOP);
    internal::utility::writeData(_boardData, Coord2D('c', 8), ChessPiece::BLACK_BISHOP);
    internal::utility::writeData(_boardData, Coord2D('f', 8), ChessPiece::BLACK_BISHOP);

    // initialize kings/queens
    internal::utility::writeData(_boardData, Coord2D('d', 1), ChessPiece::WHITE_QUEEN);
    internal::utility::writeData(_boardData, Coord2D('e', 1), ChessPiece::WHITE_KING);
    internal::utility::writeData(_boardData, Coord2D('d', 8), ChessPiece::BLACK_QUEEN);
    internal::utility::writeData(_boardData, Coord2D('e', 8), ChessPiece::BLACK_KING);

    // initialize castling rights/en passant square
    _boardData[32] = (1 << 4) - 1;
    _boardData[33] = 0;

}

ChessBoard::ChessBoard() {
    _initData();
}

ChessBoard::ChessBoard(const char* boardData) {
    std::copy(boardData, boardData + 34, _boardData);
}

std::string ChessBoard::getWhitePOV() const {
    std::string rep{"  a b c d e f g h\n"};
    
    for (int8_t row = Coord2D::BOARD_SIZE; row >= 1; --row) {
        
        rep.push_back(static_cast<char>('0' + row));
        rep.push_back('|');

        for (char col = 'a'; col <= 'h'; ++col) {

            ChessPiece piece = getPiece({ col, row });
            rep.append(piece.toAscii()); 
            rep.push_back('|');
        } 
        
        rep.push_back(static_cast<char>('0' + row));
        rep.push_back('\n');
    } 

    rep.append("  a b c d e f g h\n");

    // get white en passant and castling rights
    rep += "White en passant: ";
    rep += enpassant(Color::WHITE).has_value() ? (std::string)enpassant(Color::WHITE).value() : "none";
    rep.push_back('\n');
    rep += "White's kingside castling rights: ";
    rep += castling(Color::WHITE, false) ? "yes\n" : "no\n";
    rep += "White's queenside castling rights: ";
    rep += castling(Color::WHITE, true) ? "yes\n" : "no\n";

    return rep;
}

std::string ChessBoard::getBlackPOV() const {
    std::string rep{"  h g f e d c b a\n"};

    for (int8_t row = 1; row <= Coord2D::BOARD_SIZE; ++row) {

        rep.push_back(static_cast<char>('0' + row));
        rep.push_back('|');

        for (char col = 'h'; col >= 'a'; --col) {

            ChessPiece piece = getPiece({ col, row });
            rep.append(piece.toAscii()); 
            rep.push_back('|');
        }

        rep.push_back(static_cast<char>('0' + row));
        rep.push_back('\n');
    }

    rep.append("  h g f e d c b a\n");

    // get white en passant and castling rights
    rep += "Black en passant: ";
    rep += enpassant(Color::BLACK).has_value() ? (std::string)enpassant(Color::BLACK).value() : "none";
    rep.push_back('\n');
    rep += "Black's kingside castling rights: ";
    rep += castling(Color::BLACK, false) ? "yes\n" : "no\n";
    rep += "Black's queenside castling rights: ";
    rep += castling(Color::BLACK, true) ? "yes\n" : "no\n";

    return rep;
}

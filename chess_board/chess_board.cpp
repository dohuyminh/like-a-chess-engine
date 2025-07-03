#include "chess_board.h"
#include "chess_piece.h"
#include <stdexcept>

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
    for (int p = 8; p <= 15; ++p)  rawBoard[p] = static_cast<char>(ChessPiece::WHITE_PAWN);
    for (int p = 48; p <= 55; ++p) rawBoard[p] = static_cast<char>(ChessPiece::BLACK_PAWN);

    // assign everything else 
    rawBoard[0] = rawBoard[7] = static_cast<char>(ChessPiece::WHITE_ROOK);
    rawBoard[1] = rawBoard[6] = static_cast<char>(ChessPiece::WHITE_KNIGHT);
    rawBoard[2] = rawBoard[5] = static_cast<char>(ChessPiece::WHITE_BISHOP);
    rawBoard[3] = static_cast<char>(ChessPiece::WHITE_KING);
    rawBoard[4] = static_cast<char>(ChessPiece::WHITE_QUEEN);

    rawBoard[56] = rawBoard[63] = static_cast<char>(ChessPiece::BLACK_ROOK);
    rawBoard[57] = rawBoard[62] = static_cast<char>(ChessPiece::BLACK_KNIGHT);
    rawBoard[58] = rawBoard[61] = static_cast<char>(ChessPiece::BLACK_BISHOP);
    rawBoard[59] = static_cast<char>(ChessPiece::BLACK_KING);
    rawBoard[60] = static_cast<char>(ChessPiece::BLACK_QUEEN);

    return rawBoard;
}

ChessBoard::ChessBoard() : 
    _board(ChessBoard::initRawBoard()), 
    _whiteLeftRookMoved(false),
    _whiteRightRookMoved(false),
    _whiteKingMoved(false),
    _blackLeftRookMoved(false),
    _blackRightRookMoved(false),
    _blackKingMoved(false),
    _whiteEnpassant(std::nullopt),
    _blackEnpassant(std::nullopt) {

}

ChessBoard::ChessBoard(
        const std::string& _board, 
        bool whiteLeftRookMoved, 
        bool whiteRightRookMoved,
        bool whiteKingMoved,
        bool blackLeftRookMoved, 
        bool blackRightRookMoved,
        bool blackKingMoved,
        const std::optional<Coord2D>& _whiteEnpassant, 
        const std::optional<Coord2D>& _blackEnpassant
    ) : 
    _board(_board),
    _whiteLeftRookMoved(whiteLeftRookMoved),
    _whiteRightRookMoved(whiteRightRookMoved),
    _whiteKingMoved(whiteKingMoved),
    _blackLeftRookMoved(blackLeftRookMoved),
    _blackRightRookMoved(blackRightRookMoved),
    _blackKingMoved(blackKingMoved),
    _whiteEnpassant(_whiteEnpassant),
    _blackEnpassant(_blackEnpassant) {

}

Piece_t ChessBoard::getPiece(Coord2D coord) {
    
    if (!(coord.first >= 1 && coord.first <= 8) && (coord.second >= 'A' && coord.second <= 'H')) {
        throw std::out_of_range("Position on the board must be <1-8, A-H>");
    }

    int idx = (coord.first - 1) * ChessBoard::BOARD_SIZE + (coord.second - 'A'); 
    return _board[idx];
}


bool ChessBoard::operator==(const ChessBoard& other) {
    if (this->_board != other._board) {
        return false;
    }

    if (this->_whiteLeftRookMoved != other._whiteLeftRookMoved || this->_blackLeftRookMoved != other._blackLeftRookMoved) {
        return false;
    }

    if (this->_whiteRightRookMoved != other._whiteRightRookMoved || this->_blackRightRookMoved != other._blackRightRookMoved) {
        return false;
    }

    if (this->_whiteKingMoved != other._whiteKingMoved || this->_blackKingMoved != other._blackKingMoved) {
        return false;
    }

    if (this->_whiteEnpassant != other._whiteEnpassant || this->_blackEnpassant != other._blackEnpassant) {
        return false;
    }

    return true;
}

ChessBoard::operator std::string() {
    std::string rep{"  a b c d e f g h\n"};
    
    for (short row = ChessBoard::BOARD_SIZE - 1; row >= 0; --row) {
        
        rep.push_back('1' + row);
        rep.push_back('|');

        for (short col = ChessBoard::BOARD_SIZE - 1; col >= 0; --col) {
            
            short idx = row * ChessBoard::BOARD_SIZE + col;
            Piece_t piece = _board[idx];

            switch (piece) {
                case static_cast<char>(ChessPiece::NONE):         rep.push_back(' '); break;
                case static_cast<char>(ChessPiece::WHITE_PAWN):   rep.append("♙"); break;
                case static_cast<char>(ChessPiece::BLACK_PAWN):   rep.append("♟"); break;
                case static_cast<char>(ChessPiece::WHITE_ROOK):   rep.append("♖"); break;
                case static_cast<char>(ChessPiece::BLACK_ROOK):   rep.append("♜"); break;
                case static_cast<char>(ChessPiece::WHITE_BISHOP): rep.append("♗"); break;
                case static_cast<char>(ChessPiece::BLACK_BISHOP): rep.append("♝"); break;
                case static_cast<char>(ChessPiece::WHITE_KNIGHT): rep.append("♘"); break;
                case static_cast<char>(ChessPiece::BLACK_KNIGHT): rep.append("♞"); break;
                case static_cast<char>(ChessPiece::WHITE_QUEEN):  rep.append("♕"); break;
                case static_cast<char>(ChessPiece::BLACK_QUEEN):  rep.append("♛"); break;
                case static_cast<char>(ChessPiece::WHITE_KING):   rep.append("♔"); break;
                case static_cast<char>(ChessPiece::BLACK_KING):   rep.append("♚"); break;
            }

            rep.push_back('|');
        } 
        
        rep.push_back('1' + row);
        rep.push_back('\n');
    } 

    rep.append("  a b c d e f g h\n");

    return rep;
}
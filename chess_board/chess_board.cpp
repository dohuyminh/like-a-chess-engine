#include "chess_board.h"
#include <stdexcept>
#include <utility>

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
    _blackEnpassant(std::nullopt) {

}

ChessBoard::ChessBoard(
        std::string board,
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
    _blackEnpassant(blackEnpassant) {

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

std::unordered_set<Coord2D> ChessBoard::kingIsChecked(bool kingIsWhite) {
    
    // store all coordinates of pieces capturing the king 
    std::unordered_set<Coord2D> res;

    // scan the board for all pieces of opposite color 
    for (char col = 'A'; col <= 'H'; ++col) {
        for (int8_t row = 1; row <= 8; ++row) {
            
            Coord2D coord(col, row);
            
            // if not the color -> skip
            Piece_t currPiece = getPiece(coord);
            if (currPiece == static_cast<char>(ChessPiece::NONE) || (kingIsWhite == pieceIsWhite(currPiece))) {
                continue;
            }
            
            // scan all the squares for all possible capture; 
            // found a king -> add to collection
            if (pieceCanCaptureKing(coord)) {
                res.insert(coord);
            } 
        }
    }

    // no piece is checking the king; return false
    return res;
}

std::string ChessBoard::getWhitePOV() {
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

std::string ChessBoard::getBlackPOV() {
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

bool ChessBoard::pieceCanCaptureKing(Coord2D pieceCoord) {
    // get the current piece
    Piece_t piece = getPiece(pieceCoord);
    bool isWhite = pieceIsWhite(piece);
    Piece_t target = isWhite ? static_cast<Piece_t>(ChessPiece::BLACK_KING) : static_cast<Piece_t>(ChessPiece::WHITE_KING);

    bool isKnight = piece == static_cast<Piece_t>(ChessPiece::WHITE_KNIGHT) || piece == static_cast<Piece_t>(ChessPiece::BLACK_KNIGHT);
    bool isKing = piece == static_cast<Piece_t>(ChessPiece::WHITE_KING) || piece == static_cast<Piece_t>(ChessPiece::BLACK_KING);

    // if piece is a knight, only check up to 8 positions 
    // if it's a king instead, we also only need to check for 8 directions
    if (isKnight || isKing) {
        
        std::vector<int8_t> mvCols = { 2, 1, -2, -1, 2, 1, -2, -1 };
        std::vector<int8_t> mvRows = { 1, 2, 1, 2, -1, -2, -1, -2 };
        if (isKing) {
            mvCols = { 1, -1, 0,  0, 1,  1, 0,  0 };
            mvRows = { 0,  0, 1, -1, 1, -1, 1, -1 };
        }

        for (int8_t mvColIdx = 0; mvColIdx < 8; ++mvColIdx) {
            for (int8_t mvRowIdx = 0; mvRowIdx < 8; ++mvRowIdx) {
                Vec2D mv(mvCols[mvColIdx], mvRows[mvRowIdx]);
                try {
                    Coord2D place = pieceCoord + mv;
                    if (getPiece(place) == target) {
                        return true;
                    }
                } catch (std::invalid_argument const& e) {
                    continue;
                }
            }
        }
    }
    
    // if it's a pawn, check only at diagonal places 
    if (piece == static_cast<Piece_t>(ChessPiece::WHITE_PAWN) || piece == static_cast<Piece_t>(ChessPiece::BLACK_PAWN)) {
        
        // this state will never be reached, but just in case
        if ((isWhite && pieceCoord.row() == 8) || (!isWhite && pieceCoord.row() == 1)) {
            throw std::logic_error("FATAL ERROR: PAWNS REACHING LAST ROW SHOULD HAVE BEEN PROMOTED");
        }
        
        // check for left-side capture 
        bool left = (isWhite && pieceCoord.col() > 'A') || (!isWhite && pieceCoord.col() < 'H');
        Vec2D mvLeft = isWhite ? Vec2D(-1, 1) : Vec2D(1, -1);
        if (left && getPiece(pieceCoord + mvLeft) == target) {
            return true;
        }
        
        // check for right-side capture
        bool right = (isWhite && pieceCoord.col() < 'H') || (!isWhite && pieceCoord.col() > 'A');
        Vec2D mvRight = isWhite ? Vec2D(1, 1) : Vec2D(-1, -1);
        if (right && getPiece(pieceCoord + mvRight) == target) {
            return true;
        }
    }

    // if it's rook/bishop/queen -> start by scanning 4/8 directions and check if there is a king in sight
    bool isQueen = piece == static_cast<Piece_t>(ChessPiece::WHITE_QUEEN) || piece == static_cast<Piece_t>(ChessPiece::BLACK_QUEEN); 
    bool isRook = piece == static_cast<Piece_t>(ChessPiece::WHITE_ROOK) || piece == static_cast<Piece_t>(ChessPiece::BLACK_ROOK); 
    if (isQueen || isRook) {
        // horizontal movement
        for (char iterCol = pieceCoord.col() + 1; iterCol <= 'H'; ++iterCol) {
            Coord2D checkCoord = Coord2D(iterCol, pieceCoord.row());
            if (getPiece(checkCoord) == target) return true; 
            if (getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break; // stop if there is a piece
        }
        for (char iterCol = pieceCoord.col() - 1; iterCol >= 'A'; --iterCol) {
            Coord2D checkCoord = Coord2D(iterCol, pieceCoord.row());
            if (getPiece(checkCoord) == target) return true;
            if (getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break; 
        }

        // vertical movement
        for (int8_t iterRow = pieceCoord.row() + 1; iterRow <= 8; ++iterRow) {
            Coord2D checkCoord = Coord2D(pieceCoord.col(), iterRow);
            if (getPiece(checkCoord) == target) return true;
            if (getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break; 
        }
        for (int8_t iterRow = pieceCoord.row() - 1; iterRow >= 1; --iterRow) {
            Coord2D checkCoord = Coord2D(pieceCoord.col(), iterRow);
            if (getPiece(checkCoord) == target) return true;
            if (getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break; 
        }
    }
    
    bool isBishop = piece == static_cast<Piece_t>(ChessPiece::WHITE_BISHOP) || piece == static_cast<Piece_t>(ChessPiece::BLACK_BISHOP);
    if (isQueen || isBishop) {
        char iterCol = pieceCoord.col() + 1;
        int8_t iterRow = pieceCoord.row() + 1;

        while (iterCol <= 'H' && iterRow <= 8) {
            Coord2D checkCoord = Coord2D(iterCol, iterRow);
            if (getPiece(checkCoord) == target) return true;
            if (getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break;
            ++iterCol, ++iterRow;
        }
        
        iterCol = pieceCoord.col() + 1;
        iterRow = pieceCoord.row() - 1;
        while (iterCol <= 'H' && iterRow >= 1) {
            Coord2D checkCoord = Coord2D(iterCol, iterRow);
            if (getPiece(checkCoord) == target) return true;
            if (getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break;
            ++iterCol, --iterRow;
        }

        iterCol = pieceCoord.col() - 1;
        iterRow = pieceCoord.row() + 1;
        while (iterCol >= 'A' && iterRow <= 8) {
            Coord2D checkCoord = Coord2D(iterCol, iterRow);
            if (getPiece(checkCoord) == target) return true;
            if (getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break;
            --iterCol, ++iterRow;   
        }

        iterCol = pieceCoord.col() - 1;
        iterRow = pieceCoord.row() - 1;
        while (iterCol >= 'A' && iterRow >= 1) {
            Coord2D checkCoord = Coord2D(iterCol, iterRow);
            if (getPiece(checkCoord) == target) return true;
            if (getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break;
            --iterCol, --iterRow;
        }
    }

    // otherwise, the piece cannot capture the king immediately
    return false;
}

#include "chess_board_check.h"

#include <stdexcept>
#include <iostream>

static bool pieceCanReachSquare(const ChessBoard& board, Coord2D pieceCoord, Coord2D target) {
// get the current piece
    Piece_t piece = board.getPiece(pieceCoord);
    bool isWhite = pieceIsWhite(piece);
    
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

        for (int8_t mvIdx = 0; mvIdx < 8; ++mvIdx) {
            
            Vec2D mv(mvCols[mvIdx], mvRows[mvIdx]);
            try {
                Coord2D place = pieceCoord + mv;
                if (place == target) {
                    return true;
                }
            } catch (std::invalid_argument const& e) {
                continue;
            }
        
        }
    }
    
    // if it's a pawn, check only at diagonal places if there is a piece present
    // if the piece at the target is a pawn, also check for en passant
    if (piece == static_cast<Piece_t>(ChessPiece::WHITE_PAWN) || piece == static_cast<Piece_t>(ChessPiece::BLACK_PAWN)) {
        
        // this state will never be reached, but just in case
        if ((isWhite && pieceCoord.row() == 8) || (!isWhite && pieceCoord.row() == 1)) {
            throw std::logic_error("FATAL ERROR: PAWNS REACHING LAST ROW SHOULD HAVE BEEN PROMOTED");
        }
        
        // check for left-side capture 
        bool left = (isWhite && pieceCoord.col() > 'A') || (!isWhite && pieceCoord.col() < 'H');
        Vec2D mvLeft = isWhite ? Vec2D(-1, 1) : Vec2D(1, -1);
        if (left && pieceCoord + mvLeft == target) {
            Coord2D targetSquare = pieceCoord + mvLeft;
            if (board.getPiece(targetSquare) != static_cast<char>(ChessPiece::NONE) || 
                (isWhite && targetSquare == board.whiteEnpassant()) || 
                (!isWhite && targetSquare == board.blackEnpassant())) {
                return true;
            }
        }
        
        // check for right-side capture
        bool right = (isWhite && pieceCoord.col() < 'H') || (!isWhite && pieceCoord.col() > 'A');
        Vec2D mvRight = isWhite ? Vec2D(1, 1) : Vec2D(-1, -1);
        if (right && pieceCoord + mvRight == target) {
            Coord2D targetSquare = pieceCoord + mvLeft;
            if (board.getPiece(targetSquare) != static_cast<char>(ChessPiece::NONE) || 
                (isWhite && targetSquare == board.whiteEnpassant()) || 
                (!isWhite && targetSquare == board.blackEnpassant())) {
                return true;
            }
        }
        
        // check for forward movement 
        // if the pawn is in its original position, it may move 2 squares forward
        bool up = (isWhite && pieceCoord.row() < 8) || (!isWhite && pieceCoord.row() > 1);
        bool origin = (isWhite && pieceCoord.row() == 2) || (!isWhite && pieceCoord.row() == 7);
        Vec2D mvForward = isWhite ? Vec2D(0, 1) : Vec2D(0, -1);
        bool canMove = board.getPiece(target) == static_cast<char>(ChessPiece::NONE);
        if (canMove && ((up && pieceCoord + mvForward == target) || (origin && pieceCoord + mvForward * 2 == target))) {
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
            if (checkCoord == target) return true; 
            if (board.getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break; // stop if there is a piece
        }
        for (char iterCol = pieceCoord.col() - 1; iterCol >= 'A'; --iterCol) {
            Coord2D checkCoord = Coord2D(iterCol, pieceCoord.row());
            if (checkCoord == target) return true;
            if (board.getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break; 
        }

        // vertical movement
        for (int8_t iterRow = pieceCoord.row() + 1; iterRow <= 8; ++iterRow) {
            Coord2D checkCoord = Coord2D(pieceCoord.col(), iterRow);
            if (checkCoord == target) return true;
            if (board.getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break; 
        }
        for (int8_t iterRow = pieceCoord.row() - 1; iterRow >= 1; --iterRow) {
            Coord2D checkCoord = Coord2D(pieceCoord.col(), iterRow);
            if (checkCoord == target) return true;
            if (board.getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break; 
        }
    }
    
    bool isBishop = piece == static_cast<Piece_t>(ChessPiece::WHITE_BISHOP) || piece == static_cast<Piece_t>(ChessPiece::BLACK_BISHOP);
    if (isQueen || isBishop) {
        char iterCol = pieceCoord.col() + 1;
        int8_t iterRow = pieceCoord.row() + 1;

        while (iterCol <= 'H' && iterRow <= 8) {
            Coord2D checkCoord = Coord2D(iterCol, iterRow);
            if (checkCoord == target) return true;
            if (board.getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break;
            ++iterCol, ++iterRow;
        }
        
        iterCol = pieceCoord.col() + 1;
        iterRow = pieceCoord.row() - 1;
        while (iterCol <= 'H' && iterRow >= 1) {
            Coord2D checkCoord = Coord2D(iterCol, iterRow);
            if (checkCoord == target) return true;
            if (board.getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break;
            ++iterCol, --iterRow;
        }

        iterCol = pieceCoord.col() - 1;
        iterRow = pieceCoord.row() + 1;
        while (iterCol >= 'A' && iterRow <= 8) {
            Coord2D checkCoord = Coord2D(iterCol, iterRow);
            if (checkCoord == target) return true;
            if (board.getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break;
            --iterCol, ++iterRow;   
        }

        iterCol = pieceCoord.col() - 1;
        iterRow = pieceCoord.row() - 1;
        while (iterCol >= 'A' && iterRow >= 1) {
            Coord2D checkCoord = Coord2D(iterCol, iterRow);
            if (checkCoord == target) return true;
            if (board.getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break;
            --iterCol, --iterRow;
        }
    }

    // otherwise, the piece cannot capture the king immediately
    return false;
}

namespace CheckTerminal {
    std::unordered_set<Coord2D> kingIsChecked(const ChessBoard& board, bool kingIsWhite) {
            
        // store all coordinates of pieces capturing the king 
        std::unordered_set<Coord2D> res;

        // find the king's position
        Coord2D kingCoord = kingIsWhite ? board.whiteKingCoord() : board.blackKingCoord();

        // scan the board for all pieces of opposite color 
        for (char col = 'A'; col <= 'H'; ++col) {
            for (int8_t row = 1; row <= 8; ++row) {
                
                Coord2D coord(col, row);
                
                // if not the color -> skip
                Piece_t currPiece = board.getPiece(coord);
                if (currPiece == static_cast<char>(ChessPiece::NONE) || (kingIsWhite == pieceIsWhite(currPiece))) {
                    continue;
                }
                
                // scan all the squares for all possible capture; 
                // found a king -> add to collection
                if (pieceCanReachSquare(board, coord, kingCoord)) {
                    res.insert(coord);
                } 
            }
        }

        // no piece is checking the king; return false
        return res;
    
    }

    bool isCheckmate(const ChessBoard& board, bool kingIsWhite) {
        // if the king is not checked, it cannot be checkmate
        std::unordered_set<Coord2D> checkedPieces = kingIsChecked(board, kingIsWhite);
        if (checkedPieces.empty()) {
            return false;
        }


        Coord2D kingCoord = kingIsWhite ? board.whiteKingCoord() : board.blackKingCoord();

        // if there is only 1 piece checking the king, see if it can be captured or blocked 
        if (checkedPieces.size() == 1) {
            
            Coord2D checkingPieceCoord = *checkedPieces.begin();
            
            // check if the piece checking the king can be captured
            for (char col = 'A'; col <= 'H'; ++col) {
                for (int8_t row = 1; row <= 8; ++row) {
                    Coord2D pieceCoord(col, row);
                    Piece_t currPiece = board.getPiece(pieceCoord);
                    // if the piece is not of the same color, it can capture the checking piece
                    if (currPiece == static_cast<char>(ChessPiece::NONE) || (kingIsWhite != pieceIsWhite(currPiece))) {
                        continue;
                    }

                    // if the piece can reach the checking piece, it can capture it
                    if (pieceCanReachSquare(board, pieceCoord, checkingPieceCoord)) {
                        std::cout << "capture check\n";
                        std::cout << "Piece at " << (std::string)pieceCoord << " can capture checking piece at " << (std::string)checkingPieceCoord << std::endl;
                        return false;
                    }
                }
            }

            // check if the piece checking the king can be blocked
            // if the piece is a knight or a king, it cannot be blocked
            if (board.getPiece(checkingPieceCoord) != static_cast<char>(ChessPiece::BLACK_KNIGHT) && 
                board.getPiece(checkingPieceCoord) != static_cast<char>(ChessPiece::WHITE_KNIGHT)) {
                
                // find all the squares between the king and the checking piece
                int8_t deltaCol = (int8_t)checkingPieceCoord.col() - (int8_t)kingCoord.col(), 
                        deltaRow = checkingPieceCoord.row() - kingCoord.row();

                // either the king is checked horizontally/vertically/diagonally
                int8_t mag = std::max(std::abs(deltaCol), std::abs(deltaRow));
                Vec2D direction = Vec2D(deltaCol / mag, deltaRow / mag);
                std::cout << "Checking piece at " << (std::string)checkingPieceCoord << std::endl;
                std::cout << "King at " << (std::string)kingCoord << std::endl;
                std::cout << (int8_t)checkingPieceCoord.col() - (int8_t)kingCoord.col() << '\n';
                std::cout << "Direction:" << (std::string)direction << std::endl;

                // iterate blockable squares
                for (Coord2D blockableSquare = kingCoord + direction; blockableSquare != checkingPieceCoord; blockableSquare += direction) {
                    for (char col = 'A'; col <= 'H'; ++col) {
                        for (int8_t row = 1; row <= 8; ++row) {
                            Coord2D pieceCoord(col, row);
                            Piece_t currPiece = board.getPiece(pieceCoord);
                            // if the piece is not of the same color, it can block the checking piece
                            // for now, the king's movement is not considered 
                            if (pieceCoord == kingCoord || 
                                currPiece == static_cast<char>(ChessPiece::NONE) || 
                                (kingIsWhite != pieceIsWhite(currPiece))) {
                                continue;
                            }

                            // if the piece can reach the blockable square, it can block the checking piece
                            if (pieceCanReachSquare(board, pieceCoord, blockableSquare)) {
                                return false;
                            }
                        }
                    }
                }
            }
        }

        // if there are more pieces checking the king, or no pieces can capture/block them,
        // check if the king can move to a square that is not checked
        // check king's own move
        for (int dir = Direction::UP; dir <=Direction::DOWN_RIGHT; ++dir) {
            try {
                std::optional<ChessBoard> newState = QueensMove(kingIsWhite, static_cast<Direction>(dir), 1, kingCoord)(board);
                if (newState.has_value() && kingIsChecked(*newState, kingIsWhite).empty()) {
                    return false; // the king can move to a square that is not checked
                }
            } catch (std::invalid_argument const&) {
                continue; // move is invalid; ignore
            }
        }       

        // check castling moves
        if (kingIsWhite) {
            if (board.whiteLeftCastling()) {
                std::optional<ChessBoard> newState = Castling(true, true)(board);
                if (newState.has_value() && kingIsChecked(*newState, true).empty()) {
                    return false; // the king can move to a square that is not checked
                }
            } else if (board.whiteRightCastling()) {
                std::optional<ChessBoard> newState = Castling(true, false)(board);
                if (newState.has_value() && kingIsChecked(*newState, true).empty()) {
                    return false; // the king can move to a square that is not checked
                }
            }
        } else {
            if (board.blackLeftCastling()) {
                std::optional<ChessBoard> newState = Castling(false, true)(board);
                if (newState.has_value() && kingIsChecked(*newState, false).empty()) {
                    return false; // the king can move to a square that is not checked
                }
            } else if (board.blackRightCastling()) {
                std::optional<ChessBoard> newState = Castling(false, false)(board);
                if (newState.has_value() && kingIsChecked(*newState, false).empty()) {
                    return false; // the king can move to a square that is not checked
                }
            }
        }

        // if all else, king is checkmated
        return true;
    }
}
#include "check.h"

#include <stdexcept>

static bool pieceCanReachSquare(const ChessBoard& board, Coord2D pieceCoord, Coord2D target);

namespace CheckTerminal
{
    std::unordered_set<Coord2D> kingIsChecked(const ChessBoard& board, bool kingIsWhite) {
            
        // store all coordinates of pieces capturing the king 
        std::unordered_set<Coord2D> res;

        auto filterPiece = kingIsWhite ? pieceIsWhite : pieceIsBlack;

        // find the king's position
        Coord2D kingCoord = kingIsWhite ? board.whiteKingCoord() : board.blackKingCoord();

        // scan the board for all pieces of opposite color 
        for (char col = 'a'; col <= 'h'; ++col) {
            for (int8_t row = 1; row <= 8; ++row) {
                
                Coord2D coord(col, row);
                
                // if not the color -> skip
                Piece_t currPiece = board.getPiece(coord);
                if (filterPiece(currPiece)) {
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

    bool __pieceCanReachSquare(const ChessBoard& board, Coord2D pieceCoord, Coord2D target) {
        return pieceCanReachSquare(board, pieceCoord, target);
    }
} // namespace CheckTerminal

static bool pieceCanReachSquare(const ChessBoard& board, Coord2D pieceCoord, Coord2D target) {
    
    // get the current piece
    Piece_t piece = board.getPiece(pieceCoord);
    bool isWhite = pieceIsWhite(piece);
    
    bool isKnight = piece == static_cast<Piece_t>(ChessPiece::WHITE_KNIGHT) || piece == static_cast<Piece_t>(ChessPiece::BLACK_KNIGHT);
    bool isKing = piece == static_cast<Piece_t>(ChessPiece::WHITE_KING) || piece == static_cast<Piece_t>(ChessPiece::BLACK_KING);

    // if piece is a knight, only check up to 8 positions 
    // if it's a king instead, we also only need to check for 8 directions
    if (isKnight || isKing) {
        
        const Vec2D* mvs = isKnight ? knightsMoveVec : vecMap;

        for (int8_t mvIdx = 0; mvIdx < 8; ++mvIdx) {
            
            Vec2D mv = mvs[mvIdx];
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
        bool left = (isWhite && pieceCoord.col() > 'a') || (!isWhite && pieceCoord.col() < 'h');
        Vec2D mvLeft = isWhite ? Vec2D(-1, 1) : Vec2D(1, -1);
        if (left && pieceCoord + mvLeft == target) {
            Coord2D targetSquare = pieceCoord + mvLeft;
            Piece_t pieceAtTarget = board.getPiece(targetSquare);
            if (((pieceAtTarget != static_cast<char>(ChessPiece::NONE) && isWhite != pieceIsWhite(pieceAtTarget)) || 
                (isWhite && targetSquare == board.whiteEnpassant()) || 
                (!isWhite && targetSquare == board.blackEnpassant()))) {
                return true;
            }
        }
        
        // check for right-side capture
        bool right = (isWhite && pieceCoord.col() < 'h') || (!isWhite && pieceCoord.col() > 'a');
        Vec2D mvRight = isWhite ? Vec2D(1, 1) : Vec2D(-1, -1);
        if (right && pieceCoord + mvRight == target) {
            Coord2D targetSquare = pieceCoord + mvRight;
            Piece_t pieceAtTarget = board.getPiece(targetSquare);
            if (((pieceAtTarget != static_cast<char>(ChessPiece::NONE) && isWhite != pieceIsWhite(pieceAtTarget)) || 
                (isWhite && targetSquare == board.whiteEnpassant()) || 
                (!isWhite && targetSquare == board.blackEnpassant()))) {
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
        for (char iterCol = pieceCoord.col() + 1; iterCol <= 'h'; ++iterCol) {
            Coord2D checkCoord = Coord2D(iterCol, pieceCoord.row());
            if (checkCoord == target) return true; 
            if (board.getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break; // stop if there is a piece
        }
        for (char iterCol = pieceCoord.col() - 1; iterCol >= 'a'; --iterCol) {
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

        while (iterCol <= 'h' && iterRow <= 8) {
            Coord2D checkCoord = Coord2D(iterCol, iterRow);
            if (checkCoord == target) return true;
            if (board.getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break;
            ++iterCol, ++iterRow;
        }
        
        iterCol = pieceCoord.col() + 1;
        iterRow = pieceCoord.row() - 1;
        while (iterCol <= 'h' && iterRow >= 1) {
            Coord2D checkCoord = Coord2D(iterCol, iterRow);
            if (checkCoord == target) return true;
            if (board.getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break;
            ++iterCol, --iterRow;
        }

        iterCol = pieceCoord.col() - 1;
        iterRow = pieceCoord.row() + 1;
        while (iterCol >= 'a' && iterRow <= 8) {
            Coord2D checkCoord = Coord2D(iterCol, iterRow);
            if (checkCoord == target) return true;
            if (board.getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break;
            --iterCol, ++iterRow;   
        }

        iterCol = pieceCoord.col() - 1;
        iterRow = pieceCoord.row() - 1;
        while (iterCol >= 'a' && iterRow >= 1) {
            Coord2D checkCoord = Coord2D(iterCol, iterRow);
            if (checkCoord == target) return true;
            if (board.getPiece(checkCoord) != static_cast<char>(ChessPiece::NONE)) break;
            --iterCol, --iterRow;
        }
    }

    // otherwise, the piece cannot capture the king immediately
    return false;
}
#include "queens_move.h"
#include "../utility.h"

#include <stdexcept>

namespace internal {

/**
 * Constructor for the Queen's move. 
 * 
 * @param color The color of the piece
 * @param direction The direction at which the piece will move
 * @param numSteps How far the piece will go on the board
 * @param origin The position of the piece that will be moved
 */
QueensMove::QueensMove(Color color, Direction direction, uint8_t numSteps, Coord2D origin) :
    ChessMove(color),
    _direction(direction), 
    _numSteps(numSteps), 
    _origin(origin) {

    Vec2D mv = vecMap[_direction] * _numSteps * (_color == Color::WHITE ? 1 : -1);
    try {
        _origin + mv;
    } catch (std::invalid_argument const& e) {
        std::string err = "Queen's movement results in out-of-bound coordinate:\n";
        Coord2D cpy = origin;
        err += "Attempting to move " + (std::string)mv + " from coordinate " + (std::string)cpy + '\n'; 
        throw std::invalid_argument(err);
    }
}

/**
 * This function applies the Queen's move to the given chess board board.
 * It checks if the move is valid (i.e. the transformation is done to the correct piece), updates the board accordingly, and returns a new board.
 * If the move is invalid, it returns std::nullopt.
 * 
 * @param board The current board to be transitioned
 * @return A new board if the transition is valid, or std::nullopt otherwise
 */
std::optional<ChessBoard> QueensMove::operator()(const ChessBoard& board) const {
    // locate the position of piece on board 
    ChessPiece piece = board.getPiece(_origin);

    // if there is no piece at the square, simply return invalid
    if (piece.isNone()) {
        return std::nullopt;
    }

    // knight simply cannot perform queen's move 
    if (piece.isKnight()) {
        return std::nullopt;
    }

    // check if the transformation is done to a piece of the correct color
    if (_color != piece.color()) {
        return std::nullopt;
    }

    // get raw board 
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);

    // by default, there is no en passant; can only modify if the pawn makes a 2-square move 
    utility::turnOffEnpassant(boardData);

    // get the move transformation and the destination square
    Vec2D singleMv = vecMap[_direction] * (_color == Color::WHITE ? 1 : -1), mv = singleMv * _numSteps;
    Coord2D dest = _origin + mv;
    
    // piece cannot capture pieces of the same color
    ChessPiece pieceCheck = board.getPiece(dest);
    if (_color == pieceCheck.color()) {
        return std::nullopt;
    }
    
    // if there is a piece between origin and destination, cannot move 
    for (Coord2D iter = _origin + singleMv; iter != dest; iter = iter + singleMv) {
        if (!board.getPiece(iter).isNone()) {
            return std::nullopt;
        }
    }
    
    // only applies to pawn; end of row arrival -> queen promotion
    ChessPiece promote = board.getPiece(_origin);
    
    // pawn move 
    if (piece.isPawn()) {
        // pawn may only move 1 square ahead or 2 (at initial position) or en-passant/capture 
        // assumes queen promotion is considered
        if (_direction != Direction::UP && _direction != Direction::UP_LEFT && _direction != Direction::UP_RIGHT) {
            return std::nullopt;
        }
        
        if (_direction == Direction::UP_LEFT || _direction == Direction::UP_RIGHT) {
            if (_numSteps != 1) return std::nullopt;
            
            std::optional<Coord2D> enPassant = board.enpassant(_color);
            bool isEnpassant = enPassant == dest;

            // diagonal move may only be performed to capture pieces/enpassant
            if (board.getPiece(dest).isNone() && !isEnpassant) return std::nullopt;
            
            // enpassant requires the capture of the pawn in the corresponding square 
            if (isEnpassant) {
                Coord2D capturedPawn = dest + ((_color == Color::WHITE) ? Vec2D(0, -1) : Vec2D(0, 1)); 
                internal::utility::writeData(boardData, capturedPawn, ChessPiece::NONE);
            }
        }
        
        // pawns cannot travel more than 1 square unless they're at the original position 
        // at which en passant square shall be updated
        else {
            if (_numSteps > 2) {
                return std::nullopt;
            }
            
            // make sure nothing's in between 
            Coord2D occupiedCheck = _origin + singleMv;
            
            if (_numSteps == 2) {
                // an attempt to add 2 steps forward means there is no piece in between 
                // and piece must be in their original place 
                if ((_color == Color::WHITE && _origin.row() != 2) || (_color == Color::BLACK && _origin.row() != 7)) return std::nullopt;
                
                // a 2-block jump will make the pawn liable to enpassant
                internal::utility::setEnPassant(boardData, ~_color, occupiedCheck);
            }
            
            // pawn moving forward may not be possible if another piece is in front of it 
            else if (_numSteps == 1 && board.getPiece(occupiedCheck) != ChessPiece::NONE) return std::nullopt;
        }
        
        // if the pawn is in its last row -> promote to queen                                 
        if (_color == Color::WHITE && dest.row() == 8) promote = ChessPiece::WHITE_QUEEN;
        else if (_color == Color::BLACK && dest.row() == 1) promote = ChessPiece::BLACK_QUEEN;
    }

    // rook move 
    else if (piece.isRook()) {
        // rook may only move up/down/left/right 
        if (
            _direction != Direction::UP &&
            _direction != Direction::DOWN && 
            _direction != Direction::LEFT && 
            _direction != Direction::RIGHT
        ) return std::nullopt;
        
        // rook's move may forfeit castling rights 
        if (_color == Color::WHITE) {
            if (board.castling(_color, true) && _origin == Coord2D('a', 1)) 
                internal::utility::turnOffCastling(boardData, _color, true);
            else if (board.castling(_color, false) &&  _origin == Coord2D('h', 1)) 
                internal::utility::turnOffCastling(boardData, _color, false);;
        }
        else {
            if (board.castling(_color, true) && _origin == Coord2D('h', 8)) 
                internal::utility::turnOffCastling(boardData, _color, true);
            else if (board.castling(_color, false) && _origin == Coord2D('a', 8)) 
                internal::utility::turnOffCastling(boardData, _color, false);
        }
    }

    // bishop move 
    else if (piece.isBishop()) {
        if (
            _direction != Direction::UP_LEFT &&  
            _direction != Direction::UP_RIGHT && 
            _direction != Direction::DOWN_LEFT && 
            _direction != Direction::DOWN_RIGHT
        ) return std::nullopt;
    } 

    // king move
    else if (piece.isKing()) {
        // king may move 1 block at a time
        if (_numSteps > 1) return std::nullopt;
        
        // performing king's movement may forfeit castling rights
        internal::utility::turnOffCastling(boardData, _color, true);
        internal::utility::turnOffCastling(boardData, _color, false);
    }

    // queen's move is by default

    // update castling rights of opponent
    updateCastling(board, _color, dest, boardData);

    // perform transformation 
    internal::utility::writeData(boardData, _origin, ChessPiece::NONE);
    internal::utility::writeData(boardData, dest, promote);

    return ChessBoard(boardData); 
}

}

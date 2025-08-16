#include "queens_move.h"

#include <stdexcept>

/**
 * Constructor for the Queen's move. 
 * 
 * @param appliedPieceIsWhite Whether it is white player's turn to play (white: true, black: false)
 * @param direction The direction at which the piece will move
 * @param numSteps How far the piece will go on the board
 * @param origin The position of the piece that will be moved
 */
QueensMove::QueensMove(const bool appliedPieceIsWhite, const Direction direction, const uint8_t numSteps, const Coord2D origin) :
    ChessMove(appliedPieceIsWhite),
    _direction(direction), 
    _numSteps(numSteps), 
    _origin(origin) {

    Vec2D mv = vecMap[_direction] * _numSteps * (_isWhite ? 1 : -1);
    try {
        _origin + mv;
    } catch (std::invalid_argument const& e) {
        std::string err = "Queen's movement results in out-of-bound coordinate";
        throw std::invalid_argument(err);
    }
}

/**
 * This function applies the Queen's move to the given chess board state.
 * It checks if the move is valid (i.e. the transformation is done to the correct piece), updates the board accordingly, and returns a new state.
 * If the move is invalid, it returns std::nullopt.
 * 
 * @param state The current state to be transitioned
 * @return A new state if the transition is valid, or std::nullopt otherwise
 */
std::optional<ChessBoard> QueensMove::operator()(const ChessBoard& state) const {
    // locate the position of piece on board 
    const Piece_t piece = state.getPiece(_origin);

    // if there is no piece at the square, simply return invalid
    if (piece == static_cast<char>(ChessPiece::NONE)) {
        return std::nullopt;
    }

    // knight simply cannot perform queen's move 
    if (piece == static_cast<char>(ChessPiece::WHITE_KNIGHT) || 
        piece == static_cast<char>(ChessPiece::BLACK_KNIGHT)) {
        return std::nullopt;
    }

    // check if the transformation is done to a piece of the correct color
    if (_isWhite != pieceIsWhite(piece)) {
        return std::nullopt;
    }

    // get raw board 
    std::string rawBoard = state.board();

    // get the move transformation and the destination square
    const Vec2D singleMv = vecMap[_direction] * (_isWhite ? 1 : -1), mv = singleMv * _numSteps;
    Coord2D dest = _origin + mv;
    
    // piece cannot capture pieces of the same color
    Piece_t pieceCheck = state.getPiece(dest);
    if ((_isWhite && pieceIsWhite(pieceCheck)) || (!_isWhite && pieceIsBlack(pieceCheck))) {
        return std::nullopt;
    }
    
    // if there is a piece between origin and destination, cannot move 
    for (Coord2D iter = _origin + singleMv; iter != dest; iter = iter + singleMv) {
        if (state.getPiece(iter) != static_cast<char>(ChessPiece::NONE)) {
            return std::nullopt;
        }
    }
    
    Coord2D newWhiteKingCoord = state.whiteKingCoord(), newBlackKingCoord = state.blackKingCoord();
    
    // keep track of enpassant/castling
    bool newWhiteLeftCastling = state.whiteLeftCastling(), 
    newWhiteRightCastling = state.whiteRightCastling(), 
    newBlackLeftCastling  = state.blackLeftCastling(), 
    newBlackRightCastling = state.blackRightCastling();
    
    std::optional<Coord2D> newWhiteEnpassant = std::nullopt, newBlackEnpassant = std::nullopt;
    
    // only applies to pawn; end of row arrival -> queen promotion
    Piece_t promote = state.getPiece(_origin);
    
    // pawn move 
    if (piece == static_cast<char>(ChessPiece::WHITE_PAWN) || piece == static_cast<char>(ChessPiece::BLACK_PAWN)) {
        // pawn may only move 1 square ahead or 2 (at initial position) or en-passant/capture 
        // assumes queen promotion is considered
        if (_direction != Direction::UP && _direction != Direction::UP_LEFT && _direction != Direction::UP_RIGHT) {
            return std::nullopt;
        }
        
        if (_direction == Direction::UP_LEFT || _direction == Direction::UP_RIGHT) {
            if (_numSteps != 1) return std::nullopt;
            
            bool isEnpassant = (_isWhite && state.whiteEnpassant().has_value() && dest == state.whiteEnpassant().value()) || 
                                (!_isWhite && state.blackEnpassant().has_value() && dest == state.blackEnpassant().value());

            // diagonal move may only be performed to capture pieces/enpassant
            if (state.getPiece(dest) == static_cast<char>(ChessPiece::NONE) && !isEnpassant) return std::nullopt;
            
            // enpassant requires the capture of the pawn in the corresponding square 
            if (isEnpassant) {
                Coord2D capturedPawn = dest + ((_isWhite) ? Vec2D(0, -1) : Vec2D(0, 1)); 
                rawBoard[capturedPawn.toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
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
                if ((_isWhite && _origin.row() != 2) || (!_isWhite && _origin.row() != 7)) return std::nullopt;
                
                // a 2-block jump will make the pawn liable to enpassant
                _isWhite ? newBlackEnpassant = occupiedCheck : newWhiteEnpassant = occupiedCheck;
            }
            
            // pawn moving forward may not be possible if another piece is in front of it 
            else if (_numSteps == 1 && state.getPiece(occupiedCheck) != static_cast<char>(ChessPiece::NONE)) return std::nullopt;
        }
        
        // if the pawn is in its last row -> promote to queen                                 
        if (_isWhite && dest.row() == 8) promote = static_cast<char>(ChessPiece::WHITE_QUEEN);
        else if (!_isWhite && dest.row() == 1) promote = static_cast<char>(ChessPiece::BLACK_QUEEN);
    }

    // rook move 
    else if (piece == static_cast<char>(ChessPiece::WHITE_ROOK) || piece == static_cast<char>(ChessPiece::BLACK_ROOK)) {
        // rook may only move up/down/left/right 
        if (
            _direction != Direction::UP &&
            _direction != Direction::DOWN && 
            _direction != Direction::LEFT && 
            _direction != Direction::RIGHT
        ) return std::nullopt;
        
        // rook's move may forfeit castling rights 
        if (_isWhite) {
            if (newWhiteLeftCastling && _origin == Coord2D('a', 1)) newWhiteLeftCastling = false;
            else if (newWhiteRightCastling &&  _origin == Coord2D('h', 1)) newWhiteRightCastling = false;
        }
        else {
            if (newBlackLeftCastling && _origin == Coord2D('h', 8)) newBlackLeftCastling = false;
            else if (newBlackRightCastling && _origin == Coord2D('a', 8)) newBlackRightCastling = false;
        }
    }

    // bishop move 
    else if (piece == static_cast<char>(ChessPiece::WHITE_BISHOP) || piece == static_cast<char>(ChessPiece::BLACK_BISHOP)) {
        if (
            _direction != Direction::UP_LEFT &&  
            _direction != Direction::UP_RIGHT && 
            _direction != Direction::DOWN_LEFT && 
            _direction != Direction::DOWN_RIGHT
        ) return std::nullopt;
    } 

    // king move
    else if (piece == static_cast<char>(ChessPiece::WHITE_KING) || piece == static_cast<char>(ChessPiece::BLACK_KING)) {
        // king may move 1 block at a time
        if (_numSteps > 1) return std::nullopt;
        
        // performing king's movement may forfeit castling rights
        if (_isWhite) {
            newWhiteKingCoord = dest;
            newWhiteLeftCastling = newWhiteRightCastling = false;
        }
        else {
            newBlackKingCoord = dest;
            newBlackLeftCastling = newBlackRightCastling = false;
        }
    }

    // queen's move is by default

    // update castling rights of opponent
    updateCastling(state, _isWhite, dest, newWhiteLeftCastling, newWhiteRightCastling, newBlackLeftCastling, newBlackRightCastling);

    // perform transformation 
    rawBoard[_origin.toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    rawBoard[dest.toFlatIdx()] = promote;

    return ChessBoard(
        rawBoard, 
        newWhiteKingCoord,
        newBlackKingCoord,
        newWhiteLeftCastling, 
        newWhiteRightCastling, 
        newBlackLeftCastling, 
        newBlackRightCastling, 
        newWhiteEnpassant, 
        newBlackEnpassant
    ); 
}

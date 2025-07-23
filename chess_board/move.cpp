#include "move.h"
#include "chess_piece.h"
#include "coord2D.h"

#include <optional>
#include <stdexcept>

ChessMove::ChessMove(const bool appliedPieceIsWhite) : _isWhite(appliedPieceIsWhite) {}

/**
 * This function is used to update the opponent's castling rights, given some move might capture a rook in its initial position.
 *
 * @param state The original state to be transitioned
 * @param originPieceIsWhite Whose turn it is to play (white: true, black: false)
 * @param newPoint The new position of the piece to be moved
 * @param newWhiteLeftCastling Reference to white player's left castling's right
 * @param newWhiteRightCastling Reference to white player's right castling's right
 * @param newBlackLeftCastling Reference to black player's left castling's right
 * @param newBlackRightCastling Reference to black player's right castling's right
 */
void ChessMove::updateCastling(
    const ChessBoard& state, 
    const bool originPieceIsWhite,
    const Coord2D newPoint,
    bool& newWhiteLeftCastling, 
    bool& newWhiteRightCastling, 
    bool& newBlackLeftCastling, 
    bool& newBlackRightCastling) {

    if (originPieceIsWhite) {
        // if the rook is at the original place and castling right still exists
        // capture it and castling right is forfeit
        if (state.blackLeftCastling() && newPoint == Coord2D('H', 8)) {
            newBlackLeftCastling = false; 
        } 
        else if (state.blackRightCastling() && newPoint == Coord2D('A', 8)) {
            newBlackRightCastling = false;
        }
    }   
    else {
        // same logic applies
        if (state.whiteLeftCastling() && newPoint == Coord2D('A', 1)) {
            newWhiteLeftCastling = false; 
        } 
        else if (state.whiteRightCastling() && newPoint == Coord2D('H', 1)) {
            newWhiteRightCastling = false;
        }
    }
}

/**
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
 *
 * @param state The current state to be transitioned
 * @return A new state if the transition is valid, or std::nullopt otherwise
 */
std::optional<ChessBoard> QueensMove::operator()(const ChessBoard& state) {
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
        // pawn may only move 1 square ahead or 2 (at initial position) or en-passant   
        // assumes queen promotion is considered
        if (_direction != Direction::UP && _direction != Direction::UP_LEFT && _direction != Direction::UP_RIGHT) {
            return std::nullopt;
        }

        if (_direction == Direction::UP_LEFT || _direction == Direction::UP_RIGHT) {
            if (_numSteps != 1) return std::nullopt;

            bool isEnpassant = (_isWhite && state.blackEnpassant().has_value() && dest == state.blackEnpassant().value()) || 
                                (!_isWhite && state.whiteEnpassant().has_value() && dest == state.whiteEnpassant().value());

            // diagonal move may only be performed to capture pieces/enpassant
            if (state.getPiece(dest) == static_cast<char>(ChessPiece::NONE) && !isEnpassant) return std::nullopt;

            // enpassant requires the capture of the pawn in the corresponding square 
            if (isEnpassant) {
                Coord2D capturedPawn = dest + ((_isWhite) ? Vec2D(-1, 0) : Vec2D(1, 0)); 
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
                _isWhite ? newWhiteEnpassant = occupiedCheck : newBlackEnpassant = occupiedCheck;
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
            if (newWhiteLeftCastling && _origin == Coord2D('A', 1)) newWhiteLeftCastling = false;
            else if (newWhiteRightCastling &&  _origin == Coord2D('H', 1)) newWhiteRightCastling = false;
        }
        else {
            if (newBlackLeftCastling && _origin == Coord2D('H', 8)) newBlackLeftCastling = false;
            else if (newBlackRightCastling &&  _origin == Coord2D('A', 1)) newBlackRightCastling = false;
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
            newWhiteLeftCastling = newWhiteRightCastling = false;
        }
        else {
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
        newWhiteLeftCastling, 
        newWhiteRightCastling, 
        newBlackLeftCastling, 
        newBlackRightCastling, 
        newWhiteEnpassant, 
        newBlackEnpassant
    ); 
}

KnightsMove::KnightsMove(const bool appliedPieceIsWhite, const Vec2D direction, const Coord2D origin) :
    ChessMove(appliedPieceIsWhite),
    _direction(direction),
    _origin(origin) {

    const int8_t dx = abs(_direction.mvCol()), dy = abs(_direction.mvRow());

    // validate knight's movement
    if (dx + dy != 3 || dx < 1 || dx > 2 || dy < 1 || dy > 2) {
        throw std::invalid_argument("Knight's movement may only be <1,2>, <1,-2>, <2,1>, <2,-1>, <-1,2>, <-1,-2>, <-2,1>, <-2,-1>");
    }

    // if the knight is black -> reverse direction on board 
    if (!_isWhite) {
        _direction *= -1;
    }

    // if the movement results in out-of-bound coordinate -> throw a tantrum lol
    try {
        _origin + _direction;
    } catch(std::invalid_argument const&) {
        throw std::invalid_argument("Knight's movement results in out-of-bound coordinate");
    }
}

std::optional<ChessBoard> KnightsMove::operator()(const ChessBoard& state) {
    // locate the position of piece on board 
    const Piece_t piece = state.getPiece(_origin);

    // if there is no piece at the square, simply return invalid
    if (piece == static_cast<char>(ChessPiece::NONE)) {
        return std::nullopt;
    }
    
    // if the piece is not a knight, return invalid
    if (piece != static_cast<char>(ChessPiece::WHITE_KNIGHT) && 
        piece != static_cast<char>(ChessPiece::BLACK_KNIGHT)) {
        return std::nullopt;
    }   

    // knight is white/black
    const bool knightIsWhite = piece == static_cast<char>(ChessPiece::WHITE_KNIGHT);

    // does transformation apply to the correct piece color?
    if (knightIsWhite != _isWhite) {
        return std::nullopt;
    }

    // get the raw board
    std::string rawBoard = state.board();
        
    // get new point
    Coord2D newPoint = _origin + _direction;

    const uint8_t originIdx = _origin.toFlatIdx();
    // move
    const uint8_t destIdx = newPoint.toFlatIdx();

    // check if the position is valid (i.e. knights cannot capture pieces of the same color)
    if ((knightIsWhite && pieceIsWhite(rawBoard[destIdx])) || 
        (!knightIsWhite && pieceIsBlack(rawBoard[destIdx]))) {

        return std::nullopt;
    }
        
    // update enpassant squares (since the knight has moved, the opponent's enpassant square is invalidated)
    // since in this turn, a knight moves, there will not be any enpassant square for the home player
    std::optional<Coord2D> newWhiteEnpassant = std::nullopt;
    std::optional<Coord2D> newBlackEnpassant = std::nullopt; 
    
    // update castling rights
    bool newWhiteLeftCastling = state.whiteLeftCastling(),
        newWhiteRightCastling = state.whiteRightCastling(), 
        newBlackLeftCastling  = state.blackLeftCastling(), 
        newBlackRightCastling = state.blackRightCastling();
    
    // update castling
    updateCastling(state, knightIsWhite, newPoint, newWhiteLeftCastling, newWhiteRightCastling, newBlackLeftCastling, newBlackRightCastling);

    // transform the raw board 
    rawBoard[originIdx] = static_cast<char>(ChessPiece::NONE);
    rawBoard[destIdx] = piece;

    // return final state 
    return ChessBoard(
        rawBoard, 
        newWhiteLeftCastling, 
        newWhiteRightCastling, 
        newBlackLeftCastling, 
        newBlackRightCastling, 
        newWhiteEnpassant, 
        newBlackEnpassant
    ); 
}

Underpromotion::Underpromotion(const bool appliedPieceIsWhite, const Direction direction, const Coord2D origin, const ChessPiece promotePiece) :
    ChessMove(appliedPieceIsWhite),
    _direction(direction), 
    _origin(origin),
    _promotePiece(promotePiece)
{
    // promotion should only be done at the 2nd first/last row in the board 
    if ((!_isWhite && _origin.row() != 2) || (_isWhite && _origin.row() != 7)) {
        throw std::invalid_argument("Promotion may only apply for pawns at the 2nd last rows of their respective colors"); 
    }

    // verify if the direction is only up/up left/up right 
    if (_direction != Direction::UP && _direction != Direction::UP_LEFT && _direction != Direction::UP_RIGHT) {
        throw std::invalid_argument("Pawn may only move in the direction of Up/Up-Left/Up-Right");
    } 

    // check if pieces at the edge are trying to go out of bound 
    if (
        (_isWhite && (
            (_origin == Coord2D('A', 7) && _direction == Direction::UP_LEFT) || 
            (_origin == Coord2D('H', 7) && _direction == Direction::UP_RIGHT)
        )) || 
        (!_isWhite && (
            (_origin == Coord2D('A', 2) && _direction == Direction::UP_RIGHT) ||
            (_origin == Coord2D('H', 2) && _direction == Direction::UP_LEFT)
        ))
    ) {
        throw std::invalid_argument("Pawn's move results in out-of-bound coordinate");
    }

    // promoted pawn can only be rook/bishop/knight
    if ((!_isWhite && 
        _promotePiece != ChessPiece::BLACK_ROOK &&
        _promotePiece != ChessPiece::WHITE_ROOK && 
        _promotePiece != ChessPiece::BLACK_BISHOP) ||
        (_isWhite &&
        _promotePiece != ChessPiece::WHITE_BISHOP && 
        _promotePiece != ChessPiece::BLACK_KNIGHT && 
        _promotePiece != ChessPiece::WHITE_KNIGHT)) {

        throw std::invalid_argument("Underpromoted pawn may only be promoted to Rook/Bishop/Knight of the same color");
    }
}

std::optional<ChessBoard> Underpromotion::operator()(const ChessBoard& state) {
    // get the piece at origin 
    Piece_t piece = state.getPiece(_origin);

    // check the color of the piece 
    bool pawnIsWhite = true;
    
    // if the pawn is white -> switch
    if (piece == static_cast<char>(ChessPiece::BLACK_PAWN)) {
        pawnIsWhite = false;
    }

    // neither black/white pawn -> transformation on an invalid piece 
    else if (piece != static_cast<char>(ChessPiece::WHITE_PAWN)) {
        return std::nullopt;
    }
        
    // check if the transformation is applied to the right piece 
    if (pawnIsWhite != _isWhite) {
        return std::nullopt;
    }

    // get raw board 
    std::string rawBoard = state.board();

    // get direction and new piece position after transformation
    Vec2D dir = vecMap[_direction];
    if (!_isWhite) dir *= -1;

    Coord2D dest = _origin + dir;
    uint8_t originIdx = _origin.toFlatIdx(), destIdx = dest.toFlatIdx();

    // check if the destination is valid (i.e. pawn cannot capture piece of the same color)
    if ((_isWhite && pieceIsWhite(rawBoard[destIdx])) || 
        (!_isWhite && pieceIsBlack(rawBoard[destIdx]))) {

        return std::nullopt;
    }

    // diagonal moves may only be used for capturing pieces
    if ((_direction == Direction::UP_LEFT || _direction == Direction::UP_RIGHT) && rawBoard[destIdx] == static_cast<char>(ChessPiece::NONE)) {
        return std::nullopt;
    }

    // forward move is impossible if there is a piece in front
    if (_direction == Direction::UP && rawBoard[destIdx] != static_cast<char>(ChessPiece::NONE)) {
        return std::nullopt;
    }

    // update en passant square (since a pawn is moving at the 2nd last row, there shall be no en passant square left)
    constexpr std::optional<Coord2D> whiteEnpassant = std::nullopt;
    constexpr std::optional<Coord2D> blackEnpassant = std::nullopt;

    // update castling square 
    bool newWhiteLeftCastling = state.whiteLeftCastling(), 
        newWhiteRightCastling = state.whiteRightCastling(), 
        newBlackLeftCastling  = state.blackLeftCastling(),
        newBlackRightCastling = state.blackRightCastling();

    updateCastling(state, pawnIsWhite, dest, newWhiteLeftCastling, newWhiteRightCastling, newBlackLeftCastling, newBlackRightCastling);

    // perform transformation on raw board 
    rawBoard[originIdx] = static_cast<char>(ChessPiece::NONE);
    rawBoard[destIdx] = static_cast<char>(_promotePiece);

    return ChessBoard(
        rawBoard, 
        newWhiteLeftCastling, 
        newWhiteRightCastling, 
        newBlackLeftCastling, 
        newBlackRightCastling, 
        whiteEnpassant, 
        blackEnpassant);
}

Castling::Castling(bool isWhiteTurn, bool isLeft) : 
    ChessMove(isWhiteTurn), 
    _isLeft(isLeft) {}

std::optional<ChessBoard> Castling::operator()(const ChessBoard& state) {
    // check if castling is allowed in the first place 
    if (_isWhite) {
        if ((_isLeft && !state.whiteLeftCastling()) || (!_isLeft && !state.whiteRightCastling())) {
            return std::nullopt;
        }
    }
    if ((_isLeft && !state.blackLeftCastling()) || (!_isLeft && !state.blackRightCastling())) {
        return std::nullopt;
    }

    // navigating appropriate rook and how they should move depends on _isWhite and _isLeft
    bool navigate = (_isWhite && _isLeft) || (!_isWhite && !_isLeft);

    // check which king and which rook is going to move
    int8_t row = (_isWhite) ? 1 : 8;
    char rookCol = navigate ? 'A' : 'H';
    Vec2D mv = navigate ? Vec2D(1, 0) : Vec2D(-1, 0);
    
    Coord2D rookPos = Coord2D(rookCol, row);
    Coord2D kingPos = Coord2D('E', row);

    // get raw board 
    std::string rawBoard = state.board();

    // check if there are any pieces between the king and the rook
    for (Coord2D iter = rookPos + mv; iter != kingPos; iter = iter + mv) {
        // there exists a piece in between -> castling is impossible
        if (rawBoard[iter.toFlatIdx()] != static_cast<char>(ChessPiece::NONE)) {
            // throw std::logic_error(std::string("THE ERROR IS HERE ") + static_cast<std::string>(iter));
            return std::nullopt;
        }
    }

    // update en passant square
    std::optional<Coord2D> whiteEnpassant = std::nullopt, blackEnpassant = std::nullopt;

    // update castling rights
    bool newWhiteLeftCastling = state.whiteLeftCastling(), 
    newWhiteRightCastling = state.whiteRightCastling(), 
    newBlackLeftCastling  = state.blackLeftCastling(), 
    newBlackRightCastling = state.blackRightCastling();
    
    if (_isWhite) {
        if (_isLeft) newWhiteLeftCastling = false;
        else newWhiteRightCastling = false;
    }
    else {
        if (_isLeft) newBlackLeftCastling = false;
        else newBlackRightCastling = false;
    }
    
    // perform transformation
    Piece_t king = rawBoard[kingPos.toFlatIdx()], rook = rawBoard[rookPos.toFlatIdx()];
    
    Vec2D kingMv = mv * -2;
    Coord2D newKingPos = kingPos + kingMv;

    rawBoard[kingPos.toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    rawBoard[newKingPos.toFlatIdx()] = king;

    rawBoard[rookPos.toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    rawBoard[(newKingPos + mv).toFlatIdx()] = rook;

    return ChessBoard(
        rawBoard, 
        newWhiteLeftCastling, 
        newWhiteRightCastling, 
        newBlackLeftCastling, 
        newBlackRightCastling, 
        whiteEnpassant, 
        blackEnpassant
    );
}

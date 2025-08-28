#include "r_pawn.h"

std::vector<Coord2D> RPawn::pieceCanReachSquare(const ChessBoard& state, Coord2D origin) const {

    std::vector<Coord2D> res;

    ChessPiece pieceAtOrigin = state.getPiece(origin);
    if (pieceAtOrigin.isNone()) {
        return res;
    }

    // determine the color of the piece 
    bool originPieceIsWhite = pieceAtOrigin.isWhite(); 
    const std::optional<Coord2D>& enPassant = state.enpassant(pieceAtOrigin.color());

    // check if it can move forward 1/2 squares 
    Vec2D forward = originPieceIsWhite ? Vec2D(0, 1) : Vec2D(0, -1);

    Coord2D moveOnce = origin + forward;
    if (state.getPiece(moveOnce).isNone()) {
        res.push_back(moveOnce);

        // a piece may move 2 squares forward if it's in the original position
        // and is not obstructed 
        bool canMove2Squares = (originPieceIsWhite && origin.row() == 2) || (!originPieceIsWhite && origin.row() == 7);
        if (canMove2Squares) {
            Coord2D moveTwice = moveOnce + forward;
            if (state.getPiece(moveTwice).isNone())
                res.push_back(moveTwice);
        }
    }

    // diagonal move may only be done for capturing/en passant 
    bool canMoveLeft = (originPieceIsWhite && origin.col() > 'a') || (!originPieceIsWhite && origin.col() < 'h');
    if (canMoveLeft) {
        Vec2D mvLeft = originPieceIsWhite ? Vec2D(-1, 1) : Vec2D(1, -1);
        Coord2D nextSquare = origin + mvLeft;
        ChessPiece pieceAtNext = state.getPiece(nextSquare);
        if (pieceAtOrigin.captures(pieceAtNext) || nextSquare == enPassant) {
            res.push_back(nextSquare);
        }
    }

    bool canMoveRight = (originPieceIsWhite && origin.col() < 'h') || (!originPieceIsWhite && origin.col() > 'a');
    if (canMoveRight) {
        Vec2D mvRight = originPieceIsWhite ? Vec2D(1, 1) : Vec2D(-1, -1);
        Coord2D nextSquare = origin + mvRight;
        ChessPiece pieceAtNext = state.getPiece(nextSquare);
        if (pieceAtOrigin.captures(pieceAtNext) || nextSquare == enPassant) {
            res.push_back(nextSquare);
        }
    }

    return res;
}
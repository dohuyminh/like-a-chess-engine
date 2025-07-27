#include "move.h"
#include "chess_board.h"
#include "chess_piece.h"
#include "coord2D.h"
#include <gtest/gtest.h>

// Helper to get a board with only one piece at a location
ChessBoard boardWithPiece(Coord2D coord, ChessPiece piece, bool isWhite = true) {
    std::string raw = ChessBoard::initRawBoard();
    std::fill(raw.begin(), raw.end(), static_cast<char>(ChessPiece::NONE));
    raw[coord.toFlatIdx()] = static_cast<char>(piece);
    return ChessBoard(raw, true, true, true, true, std::nullopt, std::nullopt);
}

// ----------- QueensMove Tests -----------
TEST(QueensMoveTest, PawnMovesValidAndInvalid) {
    // White pawn single move
    ChessBoard board1 = boardWithPiece(Coord2D('D', 2), ChessPiece::WHITE_PAWN);
    QueensMove move(true, Direction::UP, 1, Coord2D('D', 2));
    auto result1 = move(board1);
    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(result1->getPiece(Coord2D('D', 3)), static_cast<char>(ChessPiece::WHITE_PAWN));

    // Black pawn single move
    ChessBoard board2 = boardWithPiece(Coord2D('D', 7), ChessPiece::BLACK_PAWN, false);
    QueensMove moveB(false, Direction::UP, 1, Coord2D('D', 7));
    auto result2 = moveB(board2);
    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(result2->getPiece(Coord2D('D', 6)), static_cast<char>(ChessPiece::BLACK_PAWN));

    // White pawn double move from initial position
    ChessBoard board3 = boardWithPiece(Coord2D('E', 2), ChessPiece::WHITE_PAWN);
    QueensMove move2(true, Direction::UP, 2, Coord2D('E', 2));
    auto result3 = move2(board3);
    ASSERT_TRUE(result3.has_value());
    EXPECT_EQ(result3->getPiece(Coord2D('E', 4)), static_cast<char>(ChessPiece::WHITE_PAWN));

    // Black pawn double move from initial position
    ChessBoard board4 = boardWithPiece(Coord2D('E', 7), ChessPiece::BLACK_PAWN, false);
    QueensMove move2B(false, Direction::UP, 2, Coord2D('E', 7));
    auto result4 = move2B(board4);
    ASSERT_TRUE(result4.has_value());
    EXPECT_EQ(result4->getPiece(Coord2D('E', 5)), static_cast<char>(ChessPiece::BLACK_PAWN));

    // Pawn cannot move sideways
    QueensMove invalid1(true, Direction::LEFT, 1, Coord2D('D', 2));
    EXPECT_FALSE(invalid1(board4).has_value());

    // Pawn cannot move more than 2 squares
    QueensMove invalid2(true, Direction::UP, 3, Coord2D('D', 2));
    EXPECT_FALSE(invalid2(board4).has_value());
}

TEST(QueensMoveTest, RookMovesValidAndInvalid) {
    // White rook up
    ChessBoard board = boardWithPiece(Coord2D('A', 1), ChessPiece::WHITE_ROOK);
    QueensMove move(true, Direction::UP, 3, Coord2D('A', 1));
    auto result = move(board);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('A', 4)), static_cast<char>(ChessPiece::WHITE_ROOK));

    // Rook cannot move diagonally
    QueensMove invalid(true, Direction::UP_RIGHT, 1, Coord2D('A', 1));
    EXPECT_FALSE(invalid(board).has_value());
}

TEST(QueensMoveTest, BishopMovesValidAndInvalid) {
    // White bishop up-right
    ChessBoard board = boardWithPiece(Coord2D('C', 1), ChessPiece::WHITE_BISHOP);
    QueensMove move(true, Direction::UP_RIGHT, 2, Coord2D('C', 1));
    auto result = move(board);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('E', 3)), static_cast<char>(ChessPiece::WHITE_BISHOP));

    // Bishop cannot move straight
    QueensMove invalid(true, Direction::UP, 1, Coord2D('C', 1));
    EXPECT_FALSE(invalid(board).has_value());
}

TEST(QueensMoveTest, QueenMovesValidAndInvalid) {
    // White queen up
    ChessBoard board = boardWithPiece(Coord2D('D', 1), ChessPiece::WHITE_QUEEN);
    QueensMove move(true, Direction::UP, 4, Coord2D('D', 1));
    auto result = move(board);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('D', 5)), static_cast<char>(ChessPiece::WHITE_QUEEN));

    // Queen diagonal
    QueensMove moveDiag(true, Direction::UP_LEFT, 2, Coord2D('D', 1));
    auto result2 = moveDiag(board);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result2->getPiece(Coord2D('B', 3)), static_cast<char>(ChessPiece::WHITE_QUEEN));
}

TEST(QueensMoveTest, KingMovesValidAndInvalid) {
    // White king up
    ChessBoard board = boardWithPiece(Coord2D('E', 1), ChessPiece::WHITE_KING);
    QueensMove move(true, Direction::UP, 1, Coord2D('E', 1));
    auto result = move(board);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('E', 2)), static_cast<char>(ChessPiece::WHITE_KING));

    // King cannot move more than 1 square
    QueensMove invalid(true, Direction::UP, 2, Coord2D('E', 1));
    EXPECT_FALSE(invalid(board).has_value());
}

TEST(QueensMoveTest, InvalidPieceType) {
    // Knight cannot use QueensMove
    ChessBoard board = boardWithPiece(Coord2D('B', 1), ChessPiece::WHITE_KNIGHT);
    QueensMove move(true, Direction::UP, 2, Coord2D('B', 1));
    EXPECT_FALSE(move(board).has_value());
}

// ----------- KnightsMove Tests -----------
TEST(KnightsMoveTest, ValidKnightMoves) {
    ChessBoard board = boardWithPiece(Coord2D('B', 1), ChessPiece::WHITE_KNIGHT);
    KnightsMove move(true, Vec2D(1, 2), Coord2D('B', 1));
    auto result = move(board);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('C', 3)), static_cast<char>(ChessPiece::WHITE_KNIGHT));
}

TEST(KnightsMoveTest, InvalidKnightMoves) {
    // Invalid vector
    EXPECT_THROW(KnightsMove(true, Vec2D(0, 0), Coord2D('B', 1)), std::invalid_argument);

    // Out of bounds
    EXPECT_THROW(KnightsMove(true, Vec2D(-2, -1), Coord2D('A', 1)), std::invalid_argument);

    // Not a knight
    ChessBoard board = boardWithPiece(Coord2D('B', 1), ChessPiece::WHITE_ROOK);
    KnightsMove move(true, Vec2D(1, 2), Coord2D('B', 1));
    EXPECT_FALSE(move(board).has_value());
}

// ----------- Underpromotion Tests -----------
TEST(UnderpromotionTest, ValidWhiteUnderpromotion) {
    ChessBoard board = boardWithPiece(Coord2D('D', 7), ChessPiece::WHITE_PAWN);
    Underpromotion move(true, Direction::UP, Coord2D('D', 7), ChessPiece::WHITE_KNIGHT);
    auto result = move(board);
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(result->getPiece(Coord2D('D', 8)), static_cast<char>(ChessPiece::WHITE_KNIGHT));
}

TEST(UnderpromotionTest, ValidBlackUnderpromotion) {
    ChessBoard board = boardWithPiece(Coord2D('E', 2), ChessPiece::BLACK_PAWN, false);
    Underpromotion move(false, Direction::UP, Coord2D('E', 2), ChessPiece::BLACK_BISHOP);
    auto result = move(board);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('E', 1)), static_cast<char>(ChessPiece::BLACK_BISHOP));
}

TEST(UnderpromotionTest, InvalidPromotionPiece) {
    ChessBoard board = boardWithPiece(Coord2D('D', 7), ChessPiece::WHITE_PAWN);
    EXPECT_THROW(Underpromotion(true, Direction::UP, Coord2D('D', 7), ChessPiece::BLACK_ROOK), std::invalid_argument);
}

TEST(UnderpromotionTest, InvalidPromotionRow) {
    ChessBoard board = boardWithPiece(Coord2D('D', 6), ChessPiece::WHITE_PAWN);
    EXPECT_THROW(Underpromotion(true, Direction::UP, Coord2D('D', 6), ChessPiece::WHITE_KNIGHT), std::invalid_argument);
}

TEST(UnderpromotionTest, InvalidPromotionDirection) {
    ChessBoard board = boardWithPiece(Coord2D('D', 7), ChessPiece::WHITE_PAWN);
    EXPECT_THROW(Underpromotion(true, Direction::LEFT, Coord2D('D', 7), ChessPiece::WHITE_KNIGHT), std::invalid_argument);
}

// ----------- Castling Tests -----------
TEST(CastlingTest, ValidWhiteLeftCastling) {
    // King at E1, rook at A1, no pieces between
    std::string raw = ChessBoard::initRawBoard();
    // Clear all between A1 and E1
    for (char col = 'B'; col < 'E'; ++col)
        raw[Coord2D(col, 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard board(raw, true, true, true, true, std::nullopt, std::nullopt);
    Castling move(true, true);
    auto result = move(board);
    ASSERT_TRUE(result.has_value());
    // King should be at C1, rook at D1
    EXPECT_EQ(result->getPiece(Coord2D('C', 1)), static_cast<char>(ChessPiece::WHITE_KING));
    EXPECT_EQ(result->getPiece(Coord2D('D', 1)), static_cast<char>(ChessPiece::WHITE_ROOK));
}

TEST(CastlingTest, ValidWhiteRightCastling) {
    std::string raw = ChessBoard::initRawBoard();
    for (char col = 'F'; col < 'H'; ++col)
        raw[Coord2D(col, 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard board(raw, true, true, true, true, std::nullopt, std::nullopt);
    Castling move(true, false);
    auto result = move(board);
    ASSERT_TRUE(result.has_value()) << board.getWhitePOV();
    EXPECT_EQ(result->getPiece(Coord2D('G', 1)), static_cast<char>(ChessPiece::WHITE_KING));
    EXPECT_EQ(result->getPiece(Coord2D('F', 1)), static_cast<char>(ChessPiece::WHITE_ROOK));
}

TEST(CastlingTest, InvalidCastlingBlocked) {
    // Piece between king and rook
    std::string raw = ChessBoard::initRawBoard();
    raw[Coord2D('B', 1).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_KNIGHT);
    ChessBoard board(raw, true, true, true, true, std::nullopt, std::nullopt);
    Castling move(true, true);
    EXPECT_FALSE(move(board).has_value());
}

TEST(CastlingTest, InvalidCastlingNoRight) {
    std::string raw = ChessBoard::initRawBoard();
    ChessBoard board(raw, false, false, true, true, std::nullopt, std::nullopt);
    Castling move(true, true);
    EXPECT_FALSE(move(board).has_value());
}

TEST(CastlingTest, ValidBlackLeftCastling) {
    std::string raw = ChessBoard::initRawBoard();
    for (char col = 'F'; col < 'H'; ++col)
        raw[Coord2D(col, 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard board(raw, true, true, true, true, std::nullopt, std::nullopt);
    Castling move(false, true);
    auto result = move(board);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('G', 8)), static_cast<char>(ChessPiece::BLACK_KING));
    EXPECT_EQ(result->getPiece(Coord2D('F', 8)), static_cast<char>(ChessPiece::BLACK_ROOK));
}

TEST(CastlingTest, ValidBlackRightCastling) {
    std::string raw = ChessBoard::initRawBoard();
    for (char col = 'B'; col < 'E'; ++col)
        raw[Coord2D(col, 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard board(raw, true, true, true, true, std::nullopt, std::nullopt);
    Castling move(false, false);
    auto result = move(board);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('C', 8)), static_cast<char>(ChessPiece::BLACK_KING));
    EXPECT_EQ(result->getPiece(Coord2D('D', 8)), static_cast<char>(ChessPiece::BLACK_ROOK));
}
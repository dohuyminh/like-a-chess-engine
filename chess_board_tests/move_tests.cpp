#include <gtest/gtest.h>
#include "move.h"

class MoveTest : public ::testing::Test {
protected:
    ChessBoard board = ChessBoard();

    void SetUp() override {}
};

TEST_F(MoveTest, QueensMoveWhiteRookValid) {
    std::string rawBoard = ChessBoard::initRawBoard();
    rawBoard[Coord2D('A', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    rawBoard[Coord2D('H', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(rawBoard, Coord2D('E', 1), Coord2D('E', 8), true, true, true, true, std::nullopt, std::nullopt);
    

    QueensMove move1(true, Direction::UP, 2, Coord2D('A', 1));
    auto result1 = move1(custom);
    EXPECT_TRUE(result1.has_value());
    EXPECT_EQ(result1->getPiece(Coord2D('A', 3)), static_cast<char>(ChessPiece::WHITE_ROOK));
    EXPECT_FALSE(result1->whiteLeftCastling());

    QueensMove move2(true, Direction::UP, 6, Coord2D('H', 1));
    auto result2 = move2(result1.value());
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(result2->getPiece(Coord2D('H', 7)), static_cast<char>(ChessPiece::WHITE_ROOK));
    EXPECT_FALSE(result2->whiteRightCastling());

    QueensMove move3(true, Direction::RIGHT, 4, Coord2D('A', 3));
    auto result3 = move3(result2.value());
    EXPECT_TRUE(result3.has_value());
    EXPECT_EQ(result3->getPiece(Coord2D('E', 3)), static_cast<char>(ChessPiece::WHITE_ROOK));

    QueensMove move4(true, Direction::LEFT, 1, Coord2D('E', 3));
    auto result4 = move4(result3.value());
    EXPECT_TRUE(result4.has_value());
    EXPECT_EQ(result4->getPiece(Coord2D('D', 3)), static_cast<char>(ChessPiece::WHITE_ROOK));
}

TEST_F(MoveTest, QueensMoveBlackRookValid) {
    std::string rawBoard = ChessBoard::initRawBoard();
    rawBoard[Coord2D('A', 7).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    rawBoard[Coord2D('H', 7).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(rawBoard, Coord2D('E', 1), Coord2D('E', 8), true, true, true, true, std::nullopt, std::nullopt);
    
    QueensMove move(false, Direction::UP, 2, Coord2D('A', 8));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('A', 6)), static_cast<char>(ChessPiece::BLACK_ROOK));
    EXPECT_FALSE(result->blackRightCastling());
}

TEST_F(MoveTest, QueensMoveWhiteBishopValid) {
    std::string b = board.board();
    b[Coord2D('D', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('E', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    QueensMove move(true, Direction::UP_RIGHT, 1, Coord2D('C', 1));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('D', 2)), static_cast<char>(ChessPiece::WHITE_BISHOP));

    ChessBoard custom2(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    QueensMove move2(true, Direction::UP_LEFT, 3, Coord2D('F', 1));
    auto result2 = move2(custom2);
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(result2->getPiece(Coord2D('C', 4)), static_cast<char>(ChessPiece::WHITE_BISHOP));
}

TEST_F(MoveTest, QueensMoveWhiteQueenCapture) {
    std::string b = board.board();
    b[Coord2D('D', 2).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    QueensMove move(true, Direction::UP, 1, Coord2D('D', 1));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('D', 2)), static_cast<char>(ChessPiece::WHITE_QUEEN));
}

TEST_F(MoveTest, QueensMoveBlocked) {
    QueensMove move(true, Direction::UP, 2, Coord2D('D', 1));
    auto result = move(board);
    EXPECT_FALSE(result.has_value());
}

TEST_F(MoveTest, QueensMoveWrongColor) {
    
    EXPECT_THROW(QueensMove move(false, Direction::UP, 1, Coord2D('D', 1));, std::invalid_argument);
}

TEST_F(MoveTest, QueensMovePawnPromotion) {
    std::string b = board.board();
    b[Coord2D('E', 7).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    QueensMove move(true, Direction::UP_RIGHT, 1, Coord2D('E', 7));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('F', 8)), static_cast<char>(ChessPiece::WHITE_QUEEN));
}

TEST_F(MoveTest, KnightsMoveWhiteValid) {
    KnightsMove move(true, Vec2D(1, 2), Coord2D('B', 1));
    auto result = move(board);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('C', 3)), static_cast<char>(ChessPiece::WHITE_KNIGHT));
}

TEST_F(MoveTest, KnightsMoveBlackValid) {
    KnightsMove move(false, Vec2D(1, 2), Coord2D('B', 8));
    auto result = move(board);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('A', 6)), static_cast<char>(ChessPiece::BLACK_KNIGHT));
}

TEST_F(MoveTest, KnightsMoveCapture) {
    std::string b = board.board();
    b[Coord2D('C', 3).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    KnightsMove move(true, Vec2D(1, 2), Coord2D('B', 1));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('C', 3)), static_cast<char>(ChessPiece::WHITE_KNIGHT));
}

TEST_F(MoveTest, KnightsMoveBlockedByOwnPiece) {
    QueensMove move1(true, Direction::UP, 1, Coord2D('C', 2));
    KnightsMove move2(true, Vec2D(1, 2), Coord2D('B', 1));
    
    auto pawnBlock = move1(board);
    ASSERT_TRUE(pawnBlock.has_value());
    auto result = move2(pawnBlock.value());
    EXPECT_FALSE(result.has_value());
}

TEST_F(MoveTest, KnightsMoveInvalidDirectionThrows) {
    EXPECT_THROW(KnightsMove(true, Vec2D(2, 2), Coord2D('B', 1)), std::invalid_argument);
}

TEST_F(MoveTest, UnderpromotionWhiteRook) {
    std::string b = board.board();
    b[Coord2D('A', 7).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    b[Coord2D('A', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    Underpromotion move(true, Direction::UP_RIGHT, Coord2D('A', 7), ChessPiece::WHITE_ROOK);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('B', 8)), static_cast<char>(ChessPiece::WHITE_ROOK));
}

TEST_F(MoveTest, UnderpromotionBlackKnight) {
    std::string b = board.board();
    b[Coord2D('H', 2).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    b[Coord2D('H', 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    Underpromotion move(false, Direction::UP, Coord2D('H', 2), ChessPiece::BLACK_KNIGHT);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('H', 1)), static_cast<char>(ChessPiece::BLACK_KNIGHT));
}

TEST_F(MoveTest, UnderpromotionInvalidRowThrows) {
    EXPECT_THROW(Underpromotion(true, Direction::UP, Coord2D('A', 5), ChessPiece::WHITE_ROOK), std::invalid_argument);
}

TEST_F(MoveTest, UnderpromotionInvalidPieceThrows) {
    std::string b = board.board();
    b[Coord2D('A', 7).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    EXPECT_THROW(Underpromotion(true, Direction::UP, Coord2D('A', 7), ChessPiece::BLACK_QUEEN), std::invalid_argument);
}

TEST_F(MoveTest, CastlingWhiteKingside) {
    std::string b = board.board();
    b[Coord2D('F', 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('G', 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, Coord2D('E', 1), Coord2D('E', 8), true, true, true, true, std::nullopt, std::nullopt);
    Castling move(true, false);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('G', 1)), static_cast<char>(ChessPiece::WHITE_KING));
}

TEST_F(MoveTest, CastlingWhiteQueenside) {
    std::string b = board.board();
    b[Coord2D('B', 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('C', 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('D', 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, Coord2D('E', 1), Coord2D('E', 8), true, true, true, true, std::nullopt, std::nullopt);
    Castling move(true, true);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('C', 1)), static_cast<char>(ChessPiece::WHITE_KING));
}

TEST_F(MoveTest, CastlingBlackKingside) {
    std::string b = board.board();
    b[Coord2D('F', 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('G', 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, Coord2D('E', 1), Coord2D('E', 8), true, true, true, true, std::nullopt, std::nullopt);
    Castling move(false, true);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('G', 8)), static_cast<char>(ChessPiece::BLACK_KING));
}

TEST_F(MoveTest, CastlingBlackQueenside) {
    std::string b = board.board();
    b[Coord2D('B', 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('C', 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('D', 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, Coord2D('E', 1), Coord2D('E', 8), true, true, true, true, std::nullopt, std::nullopt);
    Castling move(false, false);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('C', 8)), static_cast<char>(ChessPiece::BLACK_KING));
}

TEST_F(MoveTest, CastlingBlockedFails) {
    Castling move(true, false);
    auto result = move(board);
    EXPECT_FALSE(result.has_value());
}

TEST_F(MoveTest, EnPassantWhite) {
    // White pawn moves two squares, black pawn captures en passant
    std::string b = board.board();
    b[Coord2D('E', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('E', 4).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    b[Coord2D('D', 4).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, Coord2D('E', 3));
    QueensMove move(false, Direction::UP_LEFT, 1, Coord2D('D', 4));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('E', 3)), static_cast<char>(ChessPiece::BLACK_PAWN));
    EXPECT_EQ(result->getPiece(Coord2D('E', 4)), static_cast<char>(ChessPiece::NONE));
}

TEST_F(MoveTest, EnPassantBlack) {
    // Black pawn moves two squares, white pawn captures en passant
    std::string b = board.board();
    b[Coord2D('D', 7).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('D', 5).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    b[Coord2D('E', 5).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, Coord2D('D', 6), std::nullopt);
    QueensMove move(true, Direction::UP_LEFT, 1, Coord2D('E', 5));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('D', 6)), static_cast<char>(ChessPiece::WHITE_PAWN));
    EXPECT_EQ(result->getPiece(Coord2D('D', 5)), static_cast<char>(ChessPiece::NONE));
}

TEST_F(MoveTest, EnPassantNotAvailable) {
    std::string b = board.board();
    b[Coord2D('E', 4).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    b[Coord2D('D', 4).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    QueensMove move(false, Direction::UP_RIGHT, 1, Coord2D('D', 4));
    auto result = move(custom);
    EXPECT_FALSE(result.has_value());
}
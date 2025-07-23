#include "chess_board.h"
#include <gtest/gtest.h>

TEST(ChessBoardTest, DefaultConstructorEquality) {
    ChessBoard board1;
    ChessBoard board2;
    EXPECT_TRUE(board1 == board2);
}

TEST(ChessBoardTest, CustomConstructorEquality) {
    std::string board = ChessBoard::initRawBoard();
    ChessBoard b1(board, true, true, true, true, std::nullopt, std::nullopt);
    ChessBoard b2(board, true, true, true, true, std::nullopt, std::nullopt);
    EXPECT_TRUE(b1 == b2);
}

TEST(ChessBoardTest, BoardStringInequality) {
    std::string board1 = ChessBoard::initRawBoard();
    std::string board2 = board1;
    board2[0] = static_cast<char>(ChessPiece::NONE); // Remove a rook
    ChessBoard b1(board1, true, true, true, true, std::nullopt, std::nullopt);
    ChessBoard b2(board2, true, true, true, true, std::nullopt, std::nullopt);
    EXPECT_FALSE(b1 == b2);
}

TEST(ChessBoardTest, CastlingRightsInequality) {
    std::string board = ChessBoard::initRawBoard();
    ChessBoard b1(board, true, true, true, true, std::nullopt, std::nullopt);
    ChessBoard b2(board, false, true, true, true, std::nullopt, std::nullopt);
    EXPECT_FALSE(b1 == b2);
}

TEST(ChessBoardTest, EnPassantInequality) {
    std::string board = ChessBoard::initRawBoard();
    std::optional<Coord2D> enpassant = Coord2D('E', 3);
    ChessBoard b1(board, true, true, true, true, enpassant, std::nullopt);
    ChessBoard b2(board, true, true, true, true, std::nullopt, std::nullopt);
    EXPECT_FALSE(b1 == b2);
}

TEST(ChessBoardTest, GetPieceAtKnownPosition) {
    ChessBoard board;
    EXPECT_EQ(board.getPiece(Coord2D('A', 1)), static_cast<char>(ChessPiece::WHITE_ROOK));
    EXPECT_EQ(board.getPiece(Coord2D('A', 2)), static_cast<char>(ChessPiece::WHITE_PAWN));
    EXPECT_EQ(board.getPiece(Coord2D('A', 7)), static_cast<char>(ChessPiece::BLACK_PAWN));
    EXPECT_EQ(board.getPiece(Coord2D('A', 8)), static_cast<char>(ChessPiece::BLACK_ROOK));
}

TEST(ChessBoardTest, GetPieceThrowsOutOfRange) {
    ChessBoard board;
    EXPECT_THROW(board.getPiece(Coord2D('A', 0)), std::invalid_argument);
    EXPECT_THROW(board.getPiece(Coord2D('A', 9)), std::invalid_argument);
    EXPECT_THROW(board.getPiece(Coord2D('I', 1)), std::invalid_argument);
}

TEST(ChessBoardTest, GetWhitePOVContainsPieces) {
    ChessBoard board;
    std::string pov = board.getWhitePOV();
    EXPECT_NE(pov.find("♖"), std::string::npos);
    EXPECT_NE(pov.find("♙"), std::string::npos);
    EXPECT_NE(pov.find("♔"), std::string::npos);
}

TEST(ChessBoardTest, GetBlackPOVContainsPieces) {
    ChessBoard board;
    std::string pov = board.getBlackPOV();
    EXPECT_NE(pov.find("♜"), std::string::npos);
    EXPECT_NE(pov.find("♟"), std::string::npos);
    EXPECT_NE(pov.find("♚"), std::string::npos);
}
#include <gtest/gtest.h>
#include "chess_board.h"
#include "chess_board_check.h"

class ChessBoardTest : public ::testing::Test {
protected:
    ChessBoard board = ChessBoard();

    void SetUp() override {}
};

TEST_F(ChessBoardTest, InitialBoardEquality) {
    ChessBoard board2;
    EXPECT_TRUE(board == board2);
}

TEST_F(ChessBoardTest, BoardInequalityDifferentCastling) {
    ChessBoard board2 = board;
    // Simulate castling right lost
    ChessBoard board3(
        board2.board(), board2.whiteKingCoord(), board2.blackKingCoord(),
        false, true, true, true, std::nullopt, std::nullopt
    );
    EXPECT_FALSE(board == board3);
}

TEST_F(ChessBoardTest, GetPieceValid) {
    EXPECT_EQ(board.getPiece(Coord2D('A', 1)), static_cast<char>(ChessPiece::WHITE_ROOK));
    EXPECT_EQ(board.getPiece(Coord2D('E', 8)), static_cast<char>(ChessPiece::BLACK_KING));
}

TEST_F(ChessBoardTest, GetPieceInvalidThrows) {
    EXPECT_THROW(board.getPiece(Coord2D('I', 1)), std::invalid_argument);
    EXPECT_THROW(board.getPiece(Coord2D('A', 9)), std::invalid_argument);
}

TEST_F(ChessBoardTest, KingIsCheckedNone) {
    EXPECT_TRUE(CheckTerminal::kingIsChecked(board, true).empty());
    EXPECT_TRUE(CheckTerminal::kingIsChecked(board, false).empty());
}

TEST_F(ChessBoardTest, KingIsCheckedByPawn) {
    // Place black pawn in front of white king
    std::string b = board.board();
    b[Coord2D('E', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('D', 2).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    ChessBoard custom(b, Coord2D('E', 1), Coord2D('E', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, true);
    EXPECT_EQ(attackers.size(), 1);
    EXPECT_TRUE(attackers.count(Coord2D('D', 2)));
}

TEST_F(ChessBoardTest, KingIsCheckedByKnight) {
    std::string b = board.board();
    b[Coord2D('F', 3).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_KNIGHT);
    ChessBoard custom(b, Coord2D('E', 1), Coord2D('E', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, true);
    EXPECT_TRUE(attackers.count(Coord2D('F', 3)));
}

TEST_F(ChessBoardTest, KingIsCheckedByRook) {
    std::string b = board.board();
    b[Coord2D('E', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('E', 3).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_ROOK);
    ChessBoard custom(b, Coord2D('E', 1), Coord2D('E', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, true);
    EXPECT_TRUE(attackers.count(Coord2D('E', 3)));
}

TEST_F(ChessBoardTest, KingIsCheckedByBishop) {
    std::string b = board.board();
    b[Coord2D('F', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('G', 3).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_BISHOP);
    ChessBoard custom(b, Coord2D('E', 1), Coord2D('E', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, true);
    EXPECT_TRUE(attackers.count(Coord2D('G', 3)));
}

TEST_F(ChessBoardTest, KingIsCheckedByQueen) {
    std::string b = board.board();
    b[Coord2D('E', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('E', 4).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_QUEEN);
    ChessBoard custom(b, Coord2D('E', 1), Coord2D('E', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, true);
    EXPECT_TRUE(attackers.count(Coord2D('E', 4)));
}

TEST_F(ChessBoardTest, KingIsCheckedByKing) {
    std::string b = board.board();
    b[Coord2D('E', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('E', 2).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_KING);
    ChessBoard custom(b, Coord2D('E', 1), Coord2D('E', 2), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, true);
    EXPECT_TRUE(attackers.count(Coord2D('E', 2)));
}

TEST_F(ChessBoardTest, IsCheckmateFalseOnInitial) {
    EXPECT_FALSE(CheckTerminal::isCheckmate(board, true));
    EXPECT_FALSE(CheckTerminal::isCheckmate(board, false));
}

TEST_F(ChessBoardTest, IsCheckmateTrueSimple) {
    // Fool's mate
    std::string b = board.board();
    b[Coord2D('F', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('G', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('E', 7).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('D', 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('H', 4).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_QUEEN);
    ChessBoard custom(b, Coord2D('E', 1), Coord2D('E', 8), true, true, true, true, std::nullopt, std::nullopt);
    EXPECT_TRUE(CheckTerminal::isCheckmate(custom, true)) << custom.getWhitePOV();
}

TEST_F(ChessBoardTest, IsCheckmateFalseIfKingCanEscape) {
    std::string b = board.board();
    b[Coord2D('E', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('E', 3).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_ROOK);
    ChessBoard custom(b, Coord2D('E', 1), Coord2D('E', 8), true, true, true, true, std::nullopt, std::nullopt);
    EXPECT_FALSE(CheckTerminal::isCheckmate(custom, true)) << custom.getWhitePOV();
}

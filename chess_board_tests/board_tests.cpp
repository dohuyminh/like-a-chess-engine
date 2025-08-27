#include <gtest/gtest.h>
#include "chess_board.h"
#include "check_terminal/check.h"
#include "check_terminal/checkmate.h"
#include "utility.h"

using namespace internal;

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

    // turn off white queenside castling for another board
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::turnOffCastling(boardData, Color::WHITE, true);

    // Simulate castling right lost
    ChessBoard board3(boardData);

    EXPECT_FALSE(board == board3);
}

TEST_F(ChessBoardTest, GetPieceValid) {
    EXPECT_EQ(board.getPiece(Coord2D('a', 1)), ChessPiece::WHITE_ROOK);
    EXPECT_EQ(board.getPiece(Coord2D('e', 8)), ChessPiece::BLACK_KING);
}

TEST_F(ChessBoardTest, GetPieceInvalidThrows) {
    EXPECT_THROW((void) board.getPiece(Coord2D('i', 1)), std::invalid_argument);
    EXPECT_THROW((void) board.getPiece(Coord2D('a', 9)), std::invalid_argument);
}

TEST_F(ChessBoardTest, KingIsCheckedNone) {
    std::unordered_set<Coord2D> whiteKingCheck = CheckTerminal::kingIsChecked(board, Color::WHITE), 
                                blackKingCheck = CheckTerminal::kingIsChecked(board, Color::BLACK);

    EXPECT_TRUE(whiteKingCheck.empty());
    EXPECT_TRUE(blackKingCheck.empty());
}

TEST_F(ChessBoardTest, KingIsCheckedByPawn) {
    // Place black pawn in front of white king
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('e', 2), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('d', 2), ChessPiece::BLACK_PAWN);
    ChessBoard custom(boardData);
    auto attackers = CheckTerminal::kingIsChecked(custom, Color::WHITE);
    EXPECT_EQ(attackers.size(), 1);
    EXPECT_TRUE(attackers.count(Coord2D('d', 2)));
}

TEST_F(ChessBoardTest, KingIsCheckedByKnight) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('f', 3), ChessPiece::BLACK_KNIGHT);
    ChessBoard custom(boardData);
    auto attackers = CheckTerminal::kingIsChecked(custom, Color::WHITE);
    EXPECT_TRUE(attackers.count(Coord2D('f', 3)));
}

TEST_F(ChessBoardTest, KingIsCheckedByRook) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('e', 2), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('e', 3), ChessPiece::BLACK_ROOK);
    ChessBoard custom(boardData);
    auto attackers = CheckTerminal::kingIsChecked(custom, Color::WHITE);
    EXPECT_TRUE(attackers.count(Coord2D('e', 3)));
}

TEST_F(ChessBoardTest, KingIsCheckedByBishop) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('f', 2), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('g', 3), ChessPiece::BLACK_BISHOP);
    ChessBoard custom(boardData);
    auto attackers = CheckTerminal::kingIsChecked(custom, Color::WHITE);
    EXPECT_TRUE(attackers.count(Coord2D('g', 3)));
}

TEST_F(ChessBoardTest, KingIsCheckedByQueen) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('e', 2), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('e', 4), ChessPiece::BLACK_QUEEN);
    ChessBoard custom(boardData);
    auto attackers = CheckTerminal::kingIsChecked(custom, Color::WHITE);
    EXPECT_TRUE(attackers.count(Coord2D('e', 4)));
}

TEST_F(ChessBoardTest, KingIsCheckedByKing) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('e', 2), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('e', 2), ChessPiece::BLACK_KING);
    
    ChessBoard custom(boardData);
    auto attackers = CheckTerminal::kingIsChecked(custom, Color::WHITE);
    EXPECT_TRUE(attackers.count(Coord2D('e', 2)));
}

TEST_F(ChessBoardTest, IsCheckmateFalseOnInitial) {
    testing::internal::CaptureStdout();
    try {
        EXPECT_EQ(CheckTerminal::isCheckmate(board, Color::WHITE), CheckTerminal::MateStatus::NONE);
        EXPECT_EQ(CheckTerminal::isCheckmate(board, Color::BLACK), CheckTerminal::MateStatus::NONE);
    } catch (std::invalid_argument const& e) {
        std::string output = testing::internal::GetCapturedStdout();
        std::cout << "[DEBUG] Output before exception:\n" << output << std::endl;
        throw; // rethrow so test fails or you can ASSERT_* here
    }
}

TEST_F(ChessBoardTest, IsCheckmateTrueSimple) {
    // Fool's mate
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('f', 2), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('g', 2), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('e', 7), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('d', 8), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('h', 4), ChessPiece::BLACK_QUEEN);
    ChessBoard custom(boardData);
    EXPECT_EQ(CheckTerminal::isCheckmate(custom, Color::WHITE), CheckTerminal::MateStatus::CHECKMATE) << custom.getWhitePOV();
}

TEST_F(ChessBoardTest, IsCheckMateCustom1) {
    char boardData[34] = { 0 };
    utility::writeData(boardData, Coord2D('b', 1), ChessPiece::WHITE_ROOK);
    utility::writeData(boardData, Coord2D('b', 2), ChessPiece::WHITE_ROOK);
    utility::writeData(boardData, Coord2D('d', 1), ChessPiece::WHITE_ROOK);
    utility::writeData(boardData, Coord2D('c', 1), ChessPiece::WHITE_KING);
    utility::writeData(boardData, Coord2D('c', 2), ChessPiece::WHITE_BISHOP);
    utility::writeData(boardData, Coord2D('e', 1), ChessPiece::BLACK_QUEEN);
    utility::writeData(boardData, Coord2D('e', 3), ChessPiece::BLACK_QUEEN);
    utility::writeData(boardData, Coord2D('e', 8), ChessPiece::BLACK_KING);

    ChessBoard customBoard(boardData);
    EXPECT_EQ(CheckTerminal::isCheckmate(customBoard, Color::WHITE), CheckTerminal::MateStatus::CHECKMATE) << customBoard.getWhitePOV();
}

TEST_F(ChessBoardTest, IsCheckmateFalseIfKingCanEscape) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('e', 2), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('e', 3), ChessPiece::BLACK_ROOK);
    
    ChessBoard custom(boardData);
    EXPECT_EQ(CheckTerminal::isCheckmate(custom, Color::WHITE), CheckTerminal::MateStatus::CHECK) << custom.getWhitePOV();
}

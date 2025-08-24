#include <gtest/gtest.h>
#include "chess_board.h"
#include "check_terminal/check.h"
#include "check_terminal/checkmate.h"

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
    std::string b = board.board();
    b[Coord2D('e', 2).toFlatIdx()] = ChessPiece::NONE;
    b[Coord2D('d', 2).toFlatIdx()] = ChessPiece::BLACK_PAWN;
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, Color::WHITE);
    EXPECT_EQ(attackers.size(), 1);
    EXPECT_TRUE(attackers.count(Coord2D('d', 2)));
}

TEST_F(ChessBoardTest, KingIsCheckedByKnight) {
    std::string b = board.board();
    b[Coord2D('f', 3).toFlatIdx()] = ChessPiece::BLACK_KNIGHT;
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, Color::WHITE);
    EXPECT_TRUE(attackers.count(Coord2D('f', 3)));
}

TEST_F(ChessBoardTest, KingIsCheckedByRook) {
    std::string b = board.board();
    b[Coord2D('e', 2).toFlatIdx()] = ChessPiece::NONE;
    b[Coord2D('e', 3).toFlatIdx()] = ChessPiece::BLACK_ROOK;
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, Color::WHITE);
    EXPECT_TRUE(attackers.count(Coord2D('e', 3)));
}

TEST_F(ChessBoardTest, KingIsCheckedByBishop) {
    std::string b = board.board();
    b[Coord2D('f', 2).toFlatIdx()] = ChessPiece::NONE;
    b[Coord2D('g', 3).toFlatIdx()] = ChessPiece::BLACK_BISHOP;
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, Color::WHITE);
    EXPECT_TRUE(attackers.count(Coord2D('g', 3)));
}

TEST_F(ChessBoardTest, KingIsCheckedByQueen) {
    std::string b = board.board();
    b[Coord2D('e', 2).toFlatIdx()] = ChessPiece::NONE;
    b[Coord2D('e', 4).toFlatIdx()] = ChessPiece::BLACK_QUEEN;
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, Color::WHITE);
    EXPECT_TRUE(attackers.count(Coord2D('e', 4)));
}

TEST_F(ChessBoardTest, KingIsCheckedByKing) {
    std::string b = board.board();
    b[Coord2D('e', 2).toFlatIdx()] = ChessPiece::NONE;
    b[Coord2D('e', 2).toFlatIdx()] = ChessPiece::BLACK_KING;
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 2), true, true, true, true, std::nullopt, std::nullopt);
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
    std::string b = board.board();
    b[Coord2D('f', 2).toFlatIdx()] = ChessPiece::NONE;
    b[Coord2D('g', 2).toFlatIdx()] = ChessPiece::NONE;
    b[Coord2D('e', 7).toFlatIdx()] = ChessPiece::NONE;
    b[Coord2D('d', 8).toFlatIdx()] = ChessPiece::NONE;
    b[Coord2D('h', 4).toFlatIdx()] = ChessPiece::BLACK_QUEEN;
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    EXPECT_EQ(CheckTerminal::isCheckmate(custom, Color::WHITE), CheckTerminal::MateStatus::CHECKMATE) << custom.getWhitePOV();
}

TEST_F(ChessBoardTest, IsCheckMateCustom1) {
    std::string b(64, static_cast<Piece_t>(ChessPiece::NONE));
    b[Coord2D('b', 1).toFlatIdx()] = b[Coord2D('b', 2).toFlatIdx()] = b[Coord2D('d', 1).toFlatIdx()] = static_cast<Piece_t>(ChessPiece::WHITE_ROOK);
    b[Coord2D('c', 1).toFlatIdx()] = static_cast<Piece_t>(ChessPiece::WHITE_KING);
    b[Coord2D('c', 2).toFlatIdx()] = static_cast<Piece_t>(ChessPiece::WHITE_BISHOP);
    b[Coord2D('e', 1).toFlatIdx()] = b[Coord2D('e', 3).toFlatIdx()] = static_cast<Piece_t>(ChessPiece::BLACK_QUEEN);
    b[Coord2D('e', 8).toFlatIdx()] = static_cast<Piece_t>(ChessPiece::BLACK_KING);

    ChessBoard customBoard(b, Coord2D('c', 1), Coord2D('e', 8), false, false, false, false, std::nullopt, std::nullopt);
    EXPECT_EQ(CheckTerminal::isCheckmate(customBoard, Color::WHITE), CheckTerminal::MateStatus::CHECKMATE) << customBoard.getWhitePOV();
}

TEST_F(ChessBoardTest, IsCheckmateFalseIfKingCanEscape) {
    std::string b = board.board();
    b[Coord2D('e', 2).toFlatIdx()] = ChessPiece::NONE;
    b[Coord2D('e', 3).toFlatIdx()] = ChessPiece::BLACK_ROOK;
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    EXPECT_EQ(CheckTerminal::isCheckmate(custom, Color::WHITE), CheckTerminal::MateStatus::CHECK) << custom.getWhitePOV();
}

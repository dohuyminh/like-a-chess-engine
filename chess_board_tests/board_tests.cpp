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
    EXPECT_EQ(board.getPiece(Coord2D('a', 1)), static_cast<char>(ChessPiece::WHITE_ROOK));
    EXPECT_EQ(board.getPiece(Coord2D('e', 8)), static_cast<char>(ChessPiece::BLACK_KING));
}

TEST_F(ChessBoardTest, GetPieceInvalidThrows) {
    EXPECT_THROW(board.getPiece(Coord2D('i', 1)), std::invalid_argument);
    EXPECT_THROW(board.getPiece(Coord2D('a', 9)), std::invalid_argument);
}

TEST_F(ChessBoardTest, KingIsCheckedNone) {
    std::unordered_set<Coord2D> whiteKingCheck = CheckTerminal::kingIsChecked(board, true), 
                                blackKingCheck = CheckTerminal::kingIsChecked(board, false);
    
    auto errMsg = [&](bool whiteKing) {
        std::string err;
        whiteKing ? err += board.getWhitePOV() : err += board.getBlackPOV();
        if (whiteKing) {
            err += "Pieces mistakenly checking white king:\n";
            for (Coord2D c: whiteKingCheck) {
                err += (std::string)c;
                err.push_back('\n');
            }
        } else {
            err += "Pieces mistakenly checking black king:\n";
            for (Coord2D c: blackKingCheck) {
                err += (std::string)c;
                err.push_back('\n');
            }
        }
        return err; 
    };

    EXPECT_TRUE(whiteKingCheck.empty()) << errMsg(true);
    EXPECT_TRUE(blackKingCheck.empty()) << errMsg(false);
}

TEST_F(ChessBoardTest, KingIsCheckedByPawn) {
    // Place black pawn in front of white king
    std::string b = board.board();
    b[Coord2D('e', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('d', 2).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, true);
    EXPECT_EQ(attackers.size(), 1);
    EXPECT_TRUE(attackers.count(Coord2D('d', 2)));
}

TEST_F(ChessBoardTest, KingIsCheckedByKnight) {
    std::string b = board.board();
    b[Coord2D('f', 3).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_KNIGHT);
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, true);
    EXPECT_TRUE(attackers.count(Coord2D('f', 3)));
}

TEST_F(ChessBoardTest, KingIsCheckedByRook) {
    std::string b = board.board();
    b[Coord2D('e', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('e', 3).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_ROOK);
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, true);
    EXPECT_TRUE(attackers.count(Coord2D('e', 3)));
}

TEST_F(ChessBoardTest, KingIsCheckedByBishop) {
    std::string b = board.board();
    b[Coord2D('f', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('g', 3).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_BISHOP);
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, true);
    EXPECT_TRUE(attackers.count(Coord2D('g', 3)));
}

TEST_F(ChessBoardTest, KingIsCheckedByQueen) {
    std::string b = board.board();
    b[Coord2D('e', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('e', 4).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_QUEEN);
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, true);
    EXPECT_TRUE(attackers.count(Coord2D('e', 4)));
}

TEST_F(ChessBoardTest, KingIsCheckedByKing) {
    std::string b = board.board();
    b[Coord2D('e', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('e', 2).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_KING);
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 2), true, true, true, true, std::nullopt, std::nullopt);
    auto attackers = CheckTerminal::kingIsChecked(custom, true);
    EXPECT_TRUE(attackers.count(Coord2D('e', 2)));
}

TEST_F(ChessBoardTest, IsCheckmateFalseOnInitial) {
    testing::internal::CaptureStdout();
    try {
        EXPECT_EQ(CheckTerminal::isCheckmate(board, true), CheckTerminal::MateStatus::NONE);
        EXPECT_EQ(CheckTerminal::isCheckmate(board, false), CheckTerminal::MateStatus::NONE);
    } catch (std::invalid_argument const& e) {
        std::string output = testing::internal::GetCapturedStdout();
        std::cout << "[DEBUG] Output before exception:\n" << output << std::endl;
        throw; // rethrow so test fails or you can ASSERT_* here
    }
}

TEST_F(ChessBoardTest, IsCheckmateTrueSimple) {
    // Fool's mate
    std::string b = board.board();
    b[Coord2D('f', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('g', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('e', 7).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('d', 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('h', 4).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_QUEEN);
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    EXPECT_EQ(CheckTerminal::isCheckmate(custom, true), CheckTerminal::MateStatus::CHECKMATE) << custom.getWhitePOV();
}

TEST_F(ChessBoardTest, IsCheckmateFalseIfKingCanEscape) {
    std::string b = board.board();
    b[Coord2D('e', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('e', 3).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_ROOK);
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    EXPECT_EQ(CheckTerminal::isCheckmate(custom, true), CheckTerminal::MateStatus::CHECK) << custom.getWhitePOV();
}

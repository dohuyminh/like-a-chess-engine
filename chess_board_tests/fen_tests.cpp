#include <gtest/gtest.h>

#include "fen.h"

using namespace internal;

class FenTest : public ::testing::Test {
protected:
    LRULegalMoveGen gen{30};
};

TEST_F(FenTest, LoadInitialPosition) {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    State st = loadStateFromFen(fen, gen);
    EXPECT_EQ(st.turn, Color::WHITE);
    EXPECT_EQ(st.movesWithoutProgress, 0u);
    EXPECT_EQ(st.board, ChessBoard());
    EXPECT_FALSE(st.isTerminal);
}

TEST_F(FenTest, InvalidPiecePlacementTooManyCols) {
    // Two digits in a row cause overshift in a rank
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PP6PP/RNBQKBNR w KQkq - 0 1";
    EXPECT_THROW(loadStateFromFen(fen, gen), FENParsingException);
}

TEST_F(FenTest, MissingKingsRejected) {
    // Remove white king 'K'
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQQBNR w KQkq - 0 1";
    EXPECT_THROW(loadStateFromFen(fen, gen), FENParsingException);
}

TEST_F(FenTest, InvalidActivePlayer) {
    // Valid piece placement with both kings present, invalid active player token 'x'
    std::string fen = "8/8/8/8/8/8/4k3/4K3 x - - 0 1";
    EXPECT_THROW(loadStateFromFen(fen, gen), FENParsingException);
}

TEST_F(FenTest, InvalidCastlingToken) {
    // Valid piece placement with both kings present, invalid castling token 'A'
    std::string fen = "8/8/8/8/8/8/4k3/4K3 w A - 0 1";
    EXPECT_THROW(loadStateFromFen(fen, gen), FENParsingException);
}

TEST_F(FenTest, EnPassantStrictValidationNoPawnBehind) {
    // Black to move with en passant target a3 but no white pawn on a2
    // Valid piece placement with both kings present
    std::string fen = "7k/8/8/8/8/8/8/4K3 b - a3 0 1";
    EXPECT_THROW(loadStateFromFen(fen, gen), FENParsingException);
}

TEST_F(FenTest, FullMovesMustStartAtOne) {
    // Valid piece placement with both kings present, full move counter is 0 (invalid)
    std::string fen = "8/8/8/8/8/8/4k3/4K3 w - - 0 0";
    EXPECT_THROW(loadStateFromFen(fen, gen), FENParsingException);
}

TEST_F(FenTest, StalemateIsTerminalNoWinner) {
    // Verified stalemate: Black to move has no legal moves and is not in check
    // Black king a8; White queen b6; White king c6
    std::string fen = "k7/8/1QK5/8/8/8/8/8 b - - 0 1";
    State st = loadStateFromFen(fen, gen);
    EXPECT_TRUE(st.isTerminal);
    EXPECT_EQ(st.winner, Color::NONE);
    // Double-check there are no legal moves for the side to move
    auto moves = gen.get(st.board, st.turn);
    EXPECT_TRUE(moves.empty());
}



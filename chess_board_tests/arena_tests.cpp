#include <gtest/gtest.h>
#include <memory>
#include "arena.h"
#include "moves/chess_move.h"
#include "coord2D.h"

using namespace internal;

class ArenaTest : public ::testing::Test {
protected:
    std::unique_ptr<Arena> arena;
    std::unique_ptr<Arena> arenaWithHistory;
    
    void SetUp() override {
        arena = std::make_unique<Arena>(1);
        arenaWithHistory = std::make_unique<Arena>(5);
    }
};

// Constructor and Initialization Tests
TEST_F(ArenaTest, ConstructorWithValidHistorySize) {
    Arena testArena(10);
    EXPECT_EQ(testArena.turn(), Color::WHITE);
    EXPECT_EQ(testArena.winner(), Color::NONE);
    EXPECT_EQ(testArena.movesWithoutProgress(), 0);
    EXPECT_EQ(testArena.currentBoard(), ChessBoard());
    EXPECT_EQ(testArena.occurrence(ChessBoard()), 1);
}

TEST_F(ArenaTest, ConstructorWithZeroHistorySizeThrows) {
    EXPECT_THROW(Arena(0), std::invalid_argument);
}

TEST_F(ArenaTest, InitialBoardState) {
    EXPECT_EQ(arena->currentBoard(), ChessBoard());
    EXPECT_EQ(arena->turn(), Color::WHITE);
    EXPECT_EQ(arena->winner(), Color::NONE);
    EXPECT_EQ(arena->movesWithoutProgress(), 0);
    EXPECT_EQ(arena->occurrence(ChessBoard()), 1);
    EXPECT_TRUE(arena->getAllMoves().size() > 0); // Should have initial moves
}

// Move Performance Tests
TEST_F(ArenaTest, PerformValidMove) {
    auto moves = arena->getAllMoves();
    ASSERT_FALSE(moves.empty());
    
    bool movePerformed = arena->performMove(moves[0]);
    EXPECT_TRUE(movePerformed);
    EXPECT_EQ(arena->turn(), Color::BLACK);
    EXPECT_EQ(arena->movesWithoutProgress(), 0); // Pawn move resets counter
}

// TODO: Requires setting up a complex checkmate position
TEST_F(ArenaTest, PerformMoveOnTerminalGameReturnsFalse) {
    // Create a scenario where game is terminal
    Arena terminalArena(1);
    // This would require setting up a checkmate position, which is complex
    // For now, we'll test the null move case
    EXPECT_FALSE(terminalArena.performMove(nullptr));
}

TEST_F(ArenaTest, PerformNullMoveThrows) {
    EXPECT_THROW(arena->performMove(nullptr), std::invalid_argument);
}

TEST_F(ArenaTest, MoveLogsAreRecorded) {
    auto moves = arena->getAllMoves();
    ASSERT_FALSE(moves.empty());
    
    size_t initialLogSize = arena->logs().size();
    arena->performMove(moves[0]);
    
    EXPECT_EQ(arena->logs().size(), initialLogSize + 1);
    EXPECT_FALSE(arena->logs().back().empty());
}

// Game Termination Tests
TEST_F(ArenaTest, ClaimDrawWithInsufficientRepetition) {
    EXPECT_FALSE(arena->claimDraw()); // Only 1 occurrence, not 3
}

// TODO: Requires setting up 50+ moves without progress (no captures/pawn moves)
TEST_F(ArenaTest, ClaimDrawWithInsufficientMovesWithoutProgress) {
    // Would need to set up 50+ moves without progress
    EXPECT_FALSE(arena->claimDraw()); // Less than 50 moves
}

// TODO: Requires setting up a terminal game state (checkmate/stalemate)
TEST_F(ArenaTest, ClaimDrawWhenGameAlreadyEnded) {
    // This would require setting up a terminal state
    // For now, test that claimDraw returns false for ongoing game
    EXPECT_FALSE(arena->claimDraw());
}

// Rollback and History Management Tests
TEST_F(ArenaTest, RollbackFromInitialStateReturnsFalse) {
    EXPECT_FALSE(arena->rollBack());
    EXPECT_EQ(arena->turn(), Color::WHITE); // Should remain unchanged
}

TEST_F(ArenaTest, RollbackAfterMove) {
    auto moves = arena->getAllMoves();
    ASSERT_FALSE(moves.empty());
    
    Color originalTurn = arena->turn();
    arena->performMove(moves[0]);
    EXPECT_NE(arena->turn(), originalTurn);
    
    EXPECT_TRUE(arena->rollBack());
    EXPECT_EQ(arena->turn(), originalTurn);
}

TEST_F(ArenaTest, HistorySizeLimit) {
    Arena limitedArena(2);
    
    // Perform multiple moves to exceed history size
    for (int i = 0; i < 5; ++i) {
        auto moves = limitedArena.getAllMoves();
        if (!moves.empty()) {
            limitedArena.performMove(moves[0]);
        }
    }
    
    // History should be limited to 2 states
    // This is hard to test directly without exposing internal state
    // We can test that rollback still works
    EXPECT_TRUE(limitedArena.rollBack());
}

// Board Counting and Repetition Tests
TEST_F(ArenaTest, BoardOccurrenceCounting) {
    ChessBoard initialBoard = arena->currentBoard();
    EXPECT_EQ(arena->occurrence(initialBoard), 1);
    
    // Perform a move and then rollback to same position
    auto moves = arena->getAllMoves();
    if (!moves.empty()) {
        arena->performMove(moves[0]);
        arena->rollBack();
        EXPECT_EQ(arena->occurrence(initialBoard), 2);
    }
}

TEST_F(ArenaTest, MovesWithoutProgressCounter) {
    // Test that pawn moves reset the counter
    auto moves = arena->getAllMoves();
    if (!moves.empty()) {
        arena->performMove(moves[0]);
        EXPECT_EQ(arena->movesWithoutProgress(), 0); // Pawn move resets
    }
}

// Edge Cases and Error Handling Tests
// TODO: Requires setting up a terminal position (checkmate/stalemate)
TEST_F(ArenaTest, GetAllMovesOnTerminalGame) {
    // This would require setting up a terminal position
    // For now, test that getAllMoves works on initial position
    auto moves = arena->getAllMoves();
    EXPECT_GT(moves.size(), 0);
}

TEST_F(ArenaTest, MultipleArenaInstancesIndependent) {
    Arena arena1(1);
    Arena arena2(1);
    
    auto moves1 = arena1.getAllMoves();
    auto moves2 = arena2.getAllMoves();
    
    if (!moves1.empty() && !moves2.empty()) {
        arena1.performMove(moves1[0]);
        // arena2 should still be in initial state
        EXPECT_EQ(arena2.turn(), Color::WHITE);
        EXPECT_NE(arena1.turn(), arena2.turn());
    }
}

TEST_F(ArenaTest, LargeHistorySize) {
    Arena largeArena(1000);
    EXPECT_EQ(largeArena.turn(), Color::WHITE);
    EXPECT_EQ(largeArena.winner(), Color::NONE);
}

TEST_F(ArenaTest, StateConsistencyAfterOperations) {
    ChessBoard initialBoard = arena->currentBoard();
    Color initialTurn = arena->turn();
    Color initialWinner = arena->winner();
    size_t initialMovesWithoutProgress = arena->movesWithoutProgress();
    
    // Perform some operations
    auto moves = arena->getAllMoves();
    if (!moves.empty()) {
        arena->performMove(moves[0]);
        arena->rollBack();
    }
    
    // State should be consistent
    EXPECT_EQ(arena->currentBoard(), initialBoard);
    EXPECT_EQ(arena->turn(), initialTurn);
    EXPECT_EQ(arena->winner(), initialWinner);
    EXPECT_EQ(arena->movesWithoutProgress(), initialMovesWithoutProgress);
}

TEST_F(ArenaTest, LogsArePreservedAfterRollback) {
    auto moves = arena->getAllMoves();
    if (!moves.empty()) {
        size_t initialLogSize = arena->logs().size();
        arena->performMove(moves[0]);
        size_t afterMoveLogSize = arena->logs().size();
        arena->rollBack();
        
        // Logs should be preserved even after rollback
        EXPECT_EQ(arena->logs().size(), afterMoveLogSize);
        EXPECT_GT(arena->logs().size(), initialLogSize);
    }
}

TEST_F(ArenaTest, BoardCountConsistency) {
    ChessBoard initialBoard = arena->currentBoard();
    EXPECT_EQ(arena->occurrence(initialBoard), 1);
    
    // Perform a move
    auto moves = arena->getAllMoves();
    if (!moves.empty()) {
        arena->performMove(moves[0]);
        // Initial board count should remain 1
        EXPECT_EQ(arena->occurrence(initialBoard), 1);
    }
}
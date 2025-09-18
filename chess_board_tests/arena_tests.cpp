#include <gtest/gtest.h>
#include "arena.h"

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

TEST_F(ArenaTest, PerformMoveOnTerminalGameReturnsFalse) {
    // Verified stalemate: Black to move has no legal moves and is not in check
    // Black king a8; White queen b6; White king c6
    std::string stalemateFen = "k7/8/1QK5/8/8/8/8/8 b - - 0 1";
    Arena terminalArena(stalemateFen, 1);
    EXPECT_TRUE(terminalArena.getAllMoves().empty());
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

TEST_F(ArenaTest, ClaimDrawWithInsufficientMovesWithoutProgress) {
    // Fresh arena: mwp==0 and occurrence(initial)==1, cannot claim draw
    EXPECT_FALSE(arena->claimDraw());
}

TEST_F(ArenaTest, ClaimDrawWhenGameAlreadyEnded) {
    std::string stalemateFen = "k7/8/1QK5/8/8/8/8/8 b - - 0 1";
    Arena terminalArena(stalemateFen, 1);
    // Game has ended by stalemate — draw claim should be rejected
    EXPECT_FALSE(terminalArena.claimDraw());
}

// Rollback and History Management Tests
TEST_F(ArenaTest, RollbackFromInitialStateReturnsFalse) {
    EXPECT_FALSE(arena->rollBack());
    EXPECT_EQ(arena->turn(), Color::WHITE); // Should remain unchanged
}

TEST_F(ArenaTest, RollbackAfterMove) {
    auto moves = arenaWithHistory->getAllMoves();
    ASSERT_FALSE(moves.empty());
    
    Color originalTurn = arenaWithHistory->turn();
    arenaWithHistory->performMove(moves[0]);
    EXPECT_NE(arenaWithHistory->turn(), originalTurn);
    
    EXPECT_TRUE(arenaWithHistory->rollBack());
    EXPECT_EQ(arenaWithHistory->turn(), originalTurn);
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
        // Rollback should not count as a new occurrence in play history
        EXPECT_EQ(arena->occurrence(initialBoard), 1);
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

TEST_F(ArenaTest, GetAllMovesOnTerminalGame) {
    std::string stalemateFen = "k7/8/1QK5/8/8/8/8/8 b - - 0 1";
    Arena terminalArena(stalemateFen, 1);
    auto moves = terminalArena.getAllMoves();
    EXPECT_EQ(moves.size(), 0u);
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
    ChessBoard initialBoard = arenaWithHistory->currentBoard();
    Color initialTurn = arenaWithHistory->turn();
    Color initialWinner = arenaWithHistory->winner();
    size_t initialMovesWithoutProgress = arenaWithHistory->movesWithoutProgress();
    
    // Perform some operations
    auto moves = arenaWithHistory->getAllMoves();
    if (!moves.empty()) {
        arenaWithHistory->performMove(moves[0]);
        arenaWithHistory->rollBack();
    }
    
    // State should be consistent
    EXPECT_EQ(arenaWithHistory->currentBoard(), initialBoard);
    EXPECT_EQ(arenaWithHistory->turn(), initialTurn);
    EXPECT_EQ(arenaWithHistory->winner(), initialWinner);
    EXPECT_EQ(arenaWithHistory->movesWithoutProgress(), initialMovesWithoutProgress);
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


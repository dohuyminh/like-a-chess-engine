#include <gtest/gtest.h>
#include "moves/queens_move.h"
#include "moves/knights_move.h"
#include "moves/underpromotion.h"
#include "moves/castling.h"
#include "moves/get_all_moves.h"
#include "check_terminal/check.h"
#include "check_terminal/checkmate.h"
#include "moves/algebraic_notation.h"
#include "moves/queens_algebraic.h"
#include "moves/knights_algebraic.h"
#include "moves/underpromotion_algebraic.h"
#include "moves/castling_algebraic.h"

using namespace internal;

class MoveTest : public ::testing::Test {
protected:
    ChessBoard board = ChessBoard();

    void SetUp() override {}
};

TEST_F(MoveTest, QueensMoveWhiteRookValid) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('a', 2), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('h', 2), ChessPiece::NONE);
    ChessBoard custom(boardData);

    // Ra1a3
    QueensMove move1(Color::WHITE, Direction::UP, 2, Coord2D('a', 1));
    auto result1 = move1(custom);
    EXPECT_TRUE(result1.has_value());
    EXPECT_EQ(result1->getPiece(Coord2D('a', 3)), ChessPiece::WHITE_ROOK);
    EXPECT_FALSE(result1->castling(Color::WHITE, true));
    // alg notation test
    QueensAlgebraic algebraic1(move1);
    MoveResult result1algebraic = algebraic1.performMove(custom);
    EXPECT_EQ(result1algebraic.notation, "Ra1a3");

    // Rh1xh7
    QueensMove move2(Color::WHITE, Direction::UP, 6, Coord2D('h', 1));
    auto result2 = move2(result1.value());
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(result2->getPiece(Coord2D('h', 7)), ChessPiece::WHITE_ROOK);
    EXPECT_FALSE(result2->castling(Color::WHITE, false));
    // alg notation test
    QueensAlgebraic algebraic2(move2);
    MoveResult result2algebraic = algebraic2.performMove(result1.value());
    EXPECT_EQ(result2algebraic.notation, "Rh1xh7");

    // Ra3e3
    QueensMove move3(Color::WHITE, Direction::RIGHT, 4, Coord2D('a', 3));
    auto result3 = move3(result2.value());
    EXPECT_TRUE(result3.has_value());
    EXPECT_EQ(result3->getPiece(Coord2D('e', 3)), ChessPiece::WHITE_ROOK);
    // alg notation test
    QueensAlgebraic algebraic3(move3);
    MoveResult result3algebraic = algebraic3.performMove(result2.value());
    EXPECT_EQ(result3algebraic.notation, "Ra3e3");

    // Re3d3
    QueensMove move4(Color::WHITE, Direction::LEFT, 1, Coord2D('e', 3));
    auto result4 = move4(result3.value());
    EXPECT_TRUE(result4.has_value());
    EXPECT_EQ(result4->getPiece(Coord2D('d', 3)), ChessPiece::WHITE_ROOK);
    // alg notation test
    QueensAlgebraic algebraic4(move4);
    MoveResult result4algebraic = algebraic4.performMove(result3.value());
    EXPECT_EQ(result4algebraic.notation, "Re3d3");
}

TEST_F(MoveTest, QueensMoveBlackRookValid) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('a', 7), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('h', 7), ChessPiece::NONE);
    ChessBoard custom(boardData);
    
    // Ra8a6
    QueensMove move(Color::BLACK, Direction::UP, 2, Coord2D('a', 8));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('a', 6)), ChessPiece::BLACK_ROOK);
    EXPECT_FALSE(result->castling(Color::BLACK, false));
    // alg notation test
    QueensAlgebraic algebraic(move);
    MoveResult resultalgebraic = algebraic.performMove(custom);
    EXPECT_EQ(resultalgebraic.notation, "Ra8a6");
}

TEST_F(MoveTest, QueensMoveWhiteBishopValid) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('d', 2), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('e', 2), ChessPiece::NONE);
    ChessBoard custom(boardData);

    // Bc1d2
    QueensMove move(Color::WHITE, Direction::UP_RIGHT, 1, Coord2D('c', 1));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('d', 2)), ChessPiece::WHITE_BISHOP);
    // alg notation test
    QueensAlgebraic algebraic(move);
    MoveResult resultalgebraic = algebraic.performMove(custom);
    EXPECT_EQ(resultalgebraic.notation, "Bc1d2");

    // Bf1c4
    ChessBoard custom2(boardData);
    QueensMove move2(Color::WHITE, Direction::UP_LEFT, 3, Coord2D('f', 1));
    auto result2 = move2(custom2);
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(result2->getPiece(Coord2D('c', 4)), ChessPiece::WHITE_BISHOP);
    // alg notation test
    QueensAlgebraic algebraic2(move2);
    MoveResult result2algebraic = algebraic2.performMove(custom2);
    EXPECT_EQ(result2algebraic.notation, "Bf1c4");
}

TEST_F(MoveTest, QueensMoveWhiteQueenCapture) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('d', 2), ChessPiece::BLACK_PAWN);
    ChessBoard custom(boardData);

    // Qd1xd2
    QueensMove move(Color::WHITE, Direction::UP, 1, Coord2D('d', 1));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('d', 2)), ChessPiece::WHITE_QUEEN);
    // alg notation test
    QueensAlgebraic algebraic(move);
    MoveResult resultalgebraic = algebraic.performMove(custom);
    EXPECT_EQ(resultalgebraic.notation, "Qd1xd2");
}

TEST_F(MoveTest, QueensMoveBlocked) {
    QueensMove move(Color::WHITE, Direction::UP, 2, Coord2D('d', 1));
    auto result = move(board);
    EXPECT_FALSE(result.has_value());
}

TEST_F(MoveTest, QueensMoveWrongColor) {
    
    EXPECT_THROW(QueensMove move(Color::BLACK, Direction::UP, 1, Coord2D('d', 1));, std::invalid_argument);
}

TEST_F(MoveTest, QueensMovePawnPromotion) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('e', 7), ChessPiece::WHITE_PAWN);
    ChessBoard custom(boardData);
    QueensMove move(Color::WHITE, Direction::UP_RIGHT, 1, Coord2D('e', 7));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('f', 8)), ChessPiece::WHITE_QUEEN);
}

TEST_F(MoveTest, KnightsMoveWhiteValid) {
    KnightsMove move(Color::WHITE, Vec2D(1, 2), Coord2D('b', 1));
    auto result = move(board);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('c', 3)), ChessPiece::WHITE_KNIGHT);
}

TEST_F(MoveTest, KnightsMoveBlackValid) {
    KnightsMove move(Color::BLACK, Vec2D(1, 2), Coord2D('b', 8));
    auto result = move(board);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('a', 6)), ChessPiece::BLACK_KNIGHT);
}

TEST_F(MoveTest, KnightsMoveCapture) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('c', 3), ChessPiece::BLACK_PAWN);
    ChessBoard custom(boardData);
    KnightsMove move(Color::WHITE, Vec2D(1, 2), Coord2D('b', 1));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('c', 3)), ChessPiece::WHITE_KNIGHT);
}

TEST_F(MoveTest, KnightsMoveBlockedByOwnPiece) {
    QueensMove move1(Color::WHITE, Direction::UP, 1, Coord2D('c', 2));
    KnightsMove move2(Color::WHITE, Vec2D(1, 2), Coord2D('b', 1));
    
    auto pawnBlock = move1(board);
    ASSERT_TRUE(pawnBlock.has_value());
    auto result = move2(pawnBlock.value());
    EXPECT_FALSE(result.has_value());
}

TEST_F(MoveTest, KnightsMoveInvalidDirectionThrows) {
    EXPECT_THROW(KnightsMove(Color::WHITE, Vec2D(2, 2), Coord2D('b', 1)), std::invalid_argument);
}

TEST_F(MoveTest, UnderpromotionWhiteRook) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('a', 7), ChessPiece::WHITE_PAWN);
    utility::writeData(boardData, Coord2D('a', 2), ChessPiece::NONE);
    ChessBoard custom(boardData);
    Underpromotion move(Color::WHITE, Direction::UP_RIGHT, Coord2D('a', 7), ChessPiece::WHITE_ROOK);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('b', 8)), ChessPiece::WHITE_ROOK);
}

TEST_F(MoveTest, UnderpromotionBlackKnight) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('h', 2), ChessPiece::BLACK_PAWN);
    utility::writeData(boardData, Coord2D('h', 1), ChessPiece::NONE);
    ChessBoard custom(boardData);
    Underpromotion move(Color::BLACK, Direction::UP, Coord2D('h', 2), ChessPiece::BLACK_KNIGHT);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('h', 1)), ChessPiece::BLACK_KNIGHT);
}

TEST_F(MoveTest, UnderpromotionInvalidRowThrows) {
    EXPECT_THROW(Underpromotion(Color::WHITE, Direction::UP, Coord2D('a', 5), ChessPiece::WHITE_ROOK), std::invalid_argument);
}

TEST_F(MoveTest, UnderpromotionInvalidPieceThrows) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('a', 7), ChessPiece::WHITE_PAWN);
    ChessBoard custom(boardData);
    EXPECT_THROW(Underpromotion(Color::WHITE, Direction::UP, Coord2D('a', 7), ChessPiece::BLACK_QUEEN), std::invalid_argument);
}

TEST_F(MoveTest, CastlingWhiteKingside) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('f', 1), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('g', 1), ChessPiece::NONE);
    ChessBoard custom(boardData);
    Castling move(Color::WHITE, false);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('g', 1)), ChessPiece::WHITE_KING);
}

TEST_F(MoveTest, CastlingWhiteQueenside) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('b', 1), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('c', 1), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('d', 1), ChessPiece::NONE);
    ChessBoard custom(boardData);
    Castling move(Color::WHITE, true);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('c', 1)), ChessPiece::WHITE_KING);
}

TEST_F(MoveTest, CastlingBlackKingside) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('f', 8), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('g', 8), ChessPiece::NONE);
    ChessBoard custom(boardData);
    Castling move(Color::BLACK, true);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('g', 8)), ChessPiece::BLACK_KING);
}

TEST_F(MoveTest, CastlingBlackQueenside) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('b', 8), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('c', 8), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('d', 8), ChessPiece::NONE);
    ChessBoard custom(boardData);
    Castling move(Color::BLACK, false);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('c', 8)), ChessPiece::BLACK_KING);
}

TEST_F(MoveTest, CastlingBlockedFails) {
    Castling move(Color::WHITE, false);
    auto result = move(board);
    EXPECT_FALSE(result.has_value());
}

TEST_F(MoveTest, CastlingTwiceFail) {
    char boardData[34] = { 0 };
    boardData[32] = (1 << 4) - 1; // both castling rights available
    utility::writeData(boardData, Coord2D('f', 1), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('g', 1), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('e', 1), ChessPiece::WHITE_KING);
    utility::writeData(boardData, Coord2D('h', 1), ChessPiece::WHITE_ROOK);
    utility::writeData(boardData, Coord2D('a', 1), ChessPiece::WHITE_ROOK);
    utility::writeData(boardData, Coord2D('e', 8), ChessPiece::BLACK_KING);
    ChessBoard custom(boardData);
    Castling m1(Color::WHITE, false);
    auto result1 = m1(custom);
    ASSERT_TRUE(result1.has_value());
    Castling m2(Color::WHITE, true);
    auto result2 = m2(result1.value());
    EXPECT_FALSE(result2.has_value());
}

TEST_F(MoveTest, EnPassantWhite) {
    // White pawn moves two squares, black pawn captures en passant
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('e', 2), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('e', 4), ChessPiece::WHITE_PAWN);
    utility::writeData(boardData, Coord2D('d', 4), ChessPiece::BLACK_PAWN);
    utility::setEnPassant(boardData, Color::BLACK, Coord2D('e', 3));
    ChessBoard custom(boardData);
    QueensMove move(Color::BLACK, Direction::UP_LEFT, 1, Coord2D('d', 4));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('e', 3)), ChessPiece::BLACK_PAWN);
    EXPECT_EQ(result->getPiece(Coord2D('e', 4)), ChessPiece::NONE);
}

TEST_F(MoveTest, EnPassantBlack) {
    // Black pawn moves two squares, white pawn captures en passant
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('d', 7), ChessPiece::NONE);
    utility::writeData(boardData, Coord2D('d', 5), ChessPiece::BLACK_PAWN);
    utility::writeData(boardData, Coord2D('e', 5), ChessPiece::WHITE_PAWN);
    utility::setEnPassant(boardData, Color::WHITE, Coord2D('d', 6));
    ChessBoard custom(boardData);
    QueensMove move(Color::WHITE, Direction::UP_LEFT, 1, Coord2D('e', 5));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('d', 6)), ChessPiece::WHITE_PAWN);
    EXPECT_EQ(result->getPiece(Coord2D('d', 5)), ChessPiece::NONE);
}

TEST_F(MoveTest, EnPassantNotAvailable) {
    char boardData[34] = { 0 };
    std::copy(board.boardData(), board.boardData() + 34, boardData);
    utility::writeData(boardData, Coord2D('e', 4), ChessPiece::WHITE_PAWN);
    utility::writeData(boardData, Coord2D('d', 4), ChessPiece::BLACK_PAWN);
    ChessBoard custom(boardData);
    QueensMove move(Color::BLACK, Direction::UP_RIGHT, 1, Coord2D('d', 4));
    auto result = move(custom);
    EXPECT_FALSE(result.has_value());
}

// Helper: Count moves of a certain type
template<typename T>
int CountMoveType(const std::vector<std::shared_ptr<ChessMove>>& moves) {
    int count = 0;
    for (const auto& m : moves) {
        if (dynamic_cast<T*>(m.get())) ++count;
    }
    return count;
}

TEST(GetAllMoves, InitialPositionWhite) {
    ChessBoard board;
    auto moves = getAllMoves(board, Color::WHITE, true);
    // 20 moves: 16 pawn moves + 4 knight moves
    EXPECT_EQ(moves.size(), 20);
    EXPECT_EQ(CountMoveType<KnightsMove>(moves), 4);

    // ensure all moves lead to a valid state
    for (const auto& move : moves) {
        auto newState = (*move)(board);
        ASSERT_TRUE(newState.has_value());
        EXPECT_TRUE(CheckTerminal::kingIsChecked(newState.value(), Color::WHITE).empty());
    }
}

TEST(GetAllMoves, InitialPositionBlack) {
    ChessBoard board;
    auto moves = getAllMoves(board, Color::BLACK, true);
    EXPECT_EQ(moves.size(), 20);
    EXPECT_EQ(CountMoveType<KnightsMove>(moves), 4);

    // ensure all moves lead to a valid state
    for (const auto& move : moves) {
        auto newState = (*move)(board);
        ASSERT_TRUE(newState.has_value());
        EXPECT_TRUE(CheckTerminal::kingIsChecked(newState.value(), Color::WHITE).empty());
    }
}

TEST(GetAllMoves, OnlyKing) {

    char boardData[34] = { 0 };
    utility::writeData(boardData, Coord2D('e', 1), ChessPiece::WHITE_KING);
    utility::writeData(boardData, Coord2D('e', 8), ChessPiece::BLACK_KING);

    ChessBoard kingBoard(boardData);
    auto moves = getAllMoves(kingBoard, Color::WHITE, true);
    // King at E1 has 5 possible moves (D1, D2, E2, F1, F2) but only those on board
    EXPECT_EQ(moves.size(), 5);
    EXPECT_EQ(CountMoveType<QueensMove>(moves), 5);
    
    // ensure all moves lead to a valid state
    for (const auto& move : moves) {
        auto newState = (*move)(kingBoard);
        ASSERT_TRUE(newState.has_value());
        EXPECT_TRUE(CheckTerminal::kingIsChecked(newState.value(), Color::WHITE).empty());
    }
}

TEST(GetAllMoves, PawnPromotionMoves) {
    char boardData[34] = { 0 };
    utility::writeData(boardData, Coord2D('a', 7), ChessPiece::WHITE_PAWN);
    utility::writeData(boardData, Coord2D('e', 1), ChessPiece::WHITE_KING);
    utility::writeData(boardData, Coord2D('e', 8), ChessPiece::BLACK_KING);
    ChessBoard board(boardData);
    auto moves = getAllMoves(board, Color::WHITE, false);
    // Pawn at A7 can move to A8 (promotion), or capture at B8 if enemy present
    EXPECT_GE(moves.size(), 9);

    utility::writeData(boardData, Coord2D('b', 8), ChessPiece::BLACK_KNIGHT);
    ChessBoard boardWithCapture(boardData);
    auto captureMoves = getAllMoves(boardWithCapture, Color::WHITE, false);
    // Now pawn can capture at B8
    EXPECT_GE(captureMoves.size(), 13);
}

TEST(GetAllMoves, BlockedPawn) {
    char boardData[34] = { 0 };
    utility::writeData(boardData, Coord2D('a', 2), ChessPiece::WHITE_PAWN);
    utility::writeData(boardData, Coord2D('a', 3), ChessPiece::BLACK_PAWN);
    utility::writeData(boardData, Coord2D('e', 1), ChessPiece::WHITE_KING);
    utility::writeData(boardData, Coord2D('e', 8), ChessPiece::BLACK_KING);
    ChessBoard board(boardData);
    auto moves = getAllMoves(board, Color::WHITE, false);
    // Pawn is blocked, should have no moves
    EXPECT_EQ(moves.size(), 5) << board.getWhitePOV();
}

TEST(GetAllMoves, PawnEnPassant) {
    char boardData[34] = { 0 };
    utility::writeData(boardData, Coord2D('e', 4), ChessPiece::WHITE_PAWN);
    utility::writeData(boardData, Coord2D('d', 4), ChessPiece::BLACK_PAWN);
    utility::writeData(boardData, Coord2D('e', 1), ChessPiece::WHITE_KING);
    utility::writeData(boardData, Coord2D('e', 8), ChessPiece::BLACK_KING);
    utility::setEnPassant(boardData, Color::BLACK, Coord2D('e', 3));
    ChessBoard board(boardData);
    auto moves = getAllMoves(board, Color::BLACK, false);
    // Pawn at E4 can capture en passant at D3
    EXPECT_EQ(moves.size(), 7);
    EXPECT_EQ(CountMoveType<QueensMove>(moves), 7);
}

TEST(GetAllMoves, KnightJumpOverPieces) {
    char boardData[34] = { 0 };
    utility::writeData(boardData, Coord2D('b', 1), ChessPiece::WHITE_KNIGHT);
    utility::writeData(boardData, Coord2D('b', 2), ChessPiece::WHITE_PAWN);
    utility::writeData(boardData, Coord2D('c', 3), ChessPiece::BLACK_PAWN);
    utility::writeData(boardData, Coord2D('e', 1), ChessPiece::WHITE_KING);
    utility::writeData(boardData, Coord2D('e', 8), ChessPiece::BLACK_KING);
    ChessBoard board(boardData);
    auto moves = getAllMoves(board, Color::WHITE, false);
    // Knight at B1 should have 2 moves: A3, C3 (C3 is a capture)
    EXPECT_EQ(moves.size(), 11) << board.getWhitePOV();
    EXPECT_EQ(CountMoveType<KnightsMove>(moves), 3);
}

TEST(GetAllMoves, RookMoves) {
    char boardData[34] = { 0 };
    utility::writeData(boardData, Coord2D('d', 4), ChessPiece::WHITE_ROOK);
    utility::writeData(boardData, Coord2D('e', 1), ChessPiece::WHITE_KING);
    utility::writeData(boardData, Coord2D('e', 8), ChessPiece::BLACK_KING);
    ChessBoard board(boardData);
    auto moves = getAllMoves(board, Color::WHITE, false);
    // Rook at D4 should have 14 moves (7 up, 7 down, 3 left, 4 right)
    EXPECT_EQ(moves.size(), 19);
}

TEST(GetAllMoves, BishopMoves) {
    char boardData[34] = { 0 };
    utility::writeData(boardData, Coord2D('c', 1), ChessPiece::WHITE_BISHOP);
    utility::writeData(boardData, Coord2D('e', 1), ChessPiece::WHITE_KING);
    utility::writeData(boardData, Coord2D('e', 8), ChessPiece::BLACK_KING);
    ChessBoard board(boardData);
    auto moves = getAllMoves(board, Color::WHITE, false);
    // Bishop at C1 should have 7 moves (diagonals)
    EXPECT_EQ(moves.size(), 12);
}

TEST(GetAllMoves, QueenMoves) {
    char boardData[34] = { 0 };
    utility::writeData(boardData, Coord2D('d', 4), ChessPiece::WHITE_QUEEN);
    utility::writeData(boardData, Coord2D('e', 1), ChessPiece::WHITE_KING);
    utility::writeData(boardData, Coord2D('e', 8), ChessPiece::BLACK_KING);
    ChessBoard board(boardData);
    auto moves = getAllMoves(board, Color::WHITE, false);
    // Queen at D4 should have 27 moves (rook + bishop moves)
    EXPECT_EQ(moves.size(), 32);
}
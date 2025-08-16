#include <gtest/gtest.h>
#include "moves/queens_move.h"
#include "moves/knights_move.h"
#include "moves/underpromotion.h"
#include "moves/castling.h"
#include "moves/get_all_moves.h"
#include "check_terminal/check.h"
#include "check_terminal/checkmate.h"

class MoveTest : public ::testing::Test {
protected:
    ChessBoard board = ChessBoard();

    void SetUp() override {}
};

TEST_F(MoveTest, QueensMoveWhiteRookValid) {
    std::string rawBoard = board.board();
    rawBoard[Coord2D('a', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    rawBoard[Coord2D('h', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(rawBoard, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    

    QueensMove move1(true, Direction::UP, 2, Coord2D('a', 1));
    auto result1 = move1(custom);
    EXPECT_TRUE(result1.has_value());
    EXPECT_EQ(result1->getPiece(Coord2D('a', 3)), static_cast<char>(ChessPiece::WHITE_ROOK));
    EXPECT_FALSE(result1->whiteLeftCastling());

    QueensMove move2(true, Direction::UP, 6, Coord2D('h', 1));
    auto result2 = move2(result1.value());
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(result2->getPiece(Coord2D('h', 7)), static_cast<char>(ChessPiece::WHITE_ROOK));
    EXPECT_FALSE(result2->whiteRightCastling());

    QueensMove move3(true, Direction::RIGHT, 4, Coord2D('a', 3));
    auto result3 = move3(result2.value());
    EXPECT_TRUE(result3.has_value());
    EXPECT_EQ(result3->getPiece(Coord2D('e', 3)), static_cast<char>(ChessPiece::WHITE_ROOK));

    QueensMove move4(true, Direction::LEFT, 1, Coord2D('e', 3));
    auto result4 = move4(result3.value());
    EXPECT_TRUE(result4.has_value());
    EXPECT_EQ(result4->getPiece(Coord2D('d', 3)), static_cast<char>(ChessPiece::WHITE_ROOK));
}

TEST_F(MoveTest, QueensMoveBlackRookValid) {
    std::string rawBoard = board.board();
    rawBoard[Coord2D('a', 7).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    rawBoard[Coord2D('h', 7).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(rawBoard, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    
    QueensMove move(false, Direction::UP, 2, Coord2D('a', 8));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('a', 6)), static_cast<char>(ChessPiece::BLACK_ROOK));
    EXPECT_FALSE(result->blackRightCastling());
}

TEST_F(MoveTest, QueensMoveWhiteBishopValid) {
    std::string b = board.board();
    b[Coord2D('d', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('e', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    QueensMove move(true, Direction::UP_RIGHT, 1, Coord2D('c', 1));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('d', 2)), static_cast<char>(ChessPiece::WHITE_BISHOP));

    ChessBoard custom2(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    QueensMove move2(true, Direction::UP_LEFT, 3, Coord2D('f', 1));
    auto result2 = move2(custom2);
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(result2->getPiece(Coord2D('c', 4)), static_cast<char>(ChessPiece::WHITE_BISHOP));
}

TEST_F(MoveTest, QueensMoveWhiteQueenCapture) {
    std::string b = board.board();
    b[Coord2D('d', 2).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    QueensMove move(true, Direction::UP, 1, Coord2D('d', 1));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('d', 2)), static_cast<char>(ChessPiece::WHITE_QUEEN));
}

TEST_F(MoveTest, QueensMoveBlocked) {
    QueensMove move(true, Direction::UP, 2, Coord2D('d', 1));
    auto result = move(board);
    EXPECT_FALSE(result.has_value());
}

TEST_F(MoveTest, QueensMoveWrongColor) {
    
    EXPECT_THROW(QueensMove move(false, Direction::UP, 1, Coord2D('d', 1));, std::invalid_argument);
}

TEST_F(MoveTest, QueensMovePawnPromotion) {
    std::string b = board.board();
    b[Coord2D('e', 7).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    QueensMove move(true, Direction::UP_RIGHT, 1, Coord2D('e', 7));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('f', 8)), static_cast<char>(ChessPiece::WHITE_QUEEN));
}

TEST_F(MoveTest, KnightsMoveWhiteValid) {
    KnightsMove move(true, Vec2D(1, 2), Coord2D('b', 1));
    auto result = move(board);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('c', 3)), static_cast<char>(ChessPiece::WHITE_KNIGHT));
}

TEST_F(MoveTest, KnightsMoveBlackValid) {
    KnightsMove move(false, Vec2D(1, 2), Coord2D('b', 8));
    auto result = move(board);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('a', 6)), static_cast<char>(ChessPiece::BLACK_KNIGHT));
}

TEST_F(MoveTest, KnightsMoveCapture) {
    std::string b = board.board();
    b[Coord2D('c', 3).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    KnightsMove move(true, Vec2D(1, 2), Coord2D('b', 1));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('c', 3)), static_cast<char>(ChessPiece::WHITE_KNIGHT));
}

TEST_F(MoveTest, KnightsMoveBlockedByOwnPiece) {
    QueensMove move1(true, Direction::UP, 1, Coord2D('c', 2));
    KnightsMove move2(true, Vec2D(1, 2), Coord2D('b', 1));
    
    auto pawnBlock = move1(board);
    ASSERT_TRUE(pawnBlock.has_value());
    auto result = move2(pawnBlock.value());
    EXPECT_FALSE(result.has_value());
}

TEST_F(MoveTest, KnightsMoveInvalidDirectionThrows) {
    EXPECT_THROW(KnightsMove(true, Vec2D(2, 2), Coord2D('b', 1)), std::invalid_argument);
}

TEST_F(MoveTest, UnderpromotionWhiteRook) {
    std::string b = board.board();
    b[Coord2D('a', 7).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    b[Coord2D('a', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    Underpromotion move(true, Direction::UP_RIGHT, Coord2D('a', 7), ChessPiece::WHITE_ROOK);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('b', 8)), static_cast<char>(ChessPiece::WHITE_ROOK));
}

TEST_F(MoveTest, UnderpromotionBlackKnight) {
    std::string b = board.board();
    b[Coord2D('h', 2).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    b[Coord2D('h', 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    Underpromotion move(false, Direction::UP, Coord2D('h', 2), ChessPiece::BLACK_KNIGHT);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('h', 1)), static_cast<char>(ChessPiece::BLACK_KNIGHT));
}

TEST_F(MoveTest, UnderpromotionInvalidRowThrows) {
    EXPECT_THROW(Underpromotion(true, Direction::UP, Coord2D('a', 5), ChessPiece::WHITE_ROOK), std::invalid_argument);
}

TEST_F(MoveTest, UnderpromotionInvalidPieceThrows) {
    std::string b = board.board();
    b[Coord2D('a', 7).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    EXPECT_THROW(Underpromotion(true, Direction::UP, Coord2D('a', 7), ChessPiece::BLACK_QUEEN), std::invalid_argument);
}

TEST_F(MoveTest, CastlingWhiteKingside) {
    std::string b = board.board();
    b[Coord2D('f', 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('g', 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    Castling move(true, false);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('g', 1)), static_cast<char>(ChessPiece::WHITE_KING));
}

TEST_F(MoveTest, CastlingWhiteQueenside) {
    std::string b = board.board();
    b[Coord2D('b', 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('c', 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('d', 1).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    Castling move(true, true);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('c', 1)), static_cast<char>(ChessPiece::WHITE_KING));
}

TEST_F(MoveTest, CastlingBlackKingside) {
    std::string b = board.board();
    b[Coord2D('f', 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('g', 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    Castling move(false, true);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('g', 8)), static_cast<char>(ChessPiece::BLACK_KING));
}

TEST_F(MoveTest, CastlingBlackQueenside) {
    std::string b = board.board();
    b[Coord2D('b', 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('c', 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('d', 8).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    ChessBoard custom(b, Coord2D('e', 1), Coord2D('e', 8), true, true, true, true, std::nullopt, std::nullopt);
    Castling move(false, false);
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('c', 8)), static_cast<char>(ChessPiece::BLACK_KING));
}

TEST_F(MoveTest, CastlingBlockedFails) {
    Castling move(true, false);
    auto result = move(board);
    EXPECT_FALSE(result.has_value());
}

TEST_F(MoveTest, EnPassantWhite) {
    // White pawn moves two squares, black pawn captures en passant
    std::string b = board.board();
    b[Coord2D('e', 2).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('e', 4).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    b[Coord2D('d', 4).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, Coord2D('e', 3));
    QueensMove move(false, Direction::UP_LEFT, 1, Coord2D('d', 4));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('e', 3)), static_cast<char>(ChessPiece::BLACK_PAWN));
    EXPECT_EQ(result->getPiece(Coord2D('e', 4)), static_cast<char>(ChessPiece::NONE));
}

TEST_F(MoveTest, EnPassantBlack) {
    // Black pawn moves two squares, white pawn captures en passant
    std::string b = board.board();
    b[Coord2D('d', 7).toFlatIdx()] = static_cast<char>(ChessPiece::NONE);
    b[Coord2D('d', 5).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    b[Coord2D('e', 5).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, Coord2D('d', 6), std::nullopt);
    QueensMove move(true, Direction::UP_LEFT, 1, Coord2D('e', 5));
    auto result = move(custom);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPiece(Coord2D('d', 6)), static_cast<char>(ChessPiece::WHITE_PAWN));
    EXPECT_EQ(result->getPiece(Coord2D('d', 5)), static_cast<char>(ChessPiece::NONE));
}

TEST_F(MoveTest, EnPassantNotAvailable) {
    std::string b = board.board();
    b[Coord2D('e', 4).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    b[Coord2D('d', 4).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    ChessBoard custom(b, board.whiteKingCoord(), board.blackKingCoord(), true, true, true, true, std::nullopt, std::nullopt);
    QueensMove move(false, Direction::UP_RIGHT, 1, Coord2D('d', 4));
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
    auto moves = getAllMoves(board, true, true);
    // 20 moves: 16 pawn moves + 4 knight moves
    EXPECT_EQ(moves.size(), 20);
    EXPECT_EQ(CountMoveType<KnightsMove>(moves), 4);

    // ensure all moves lead to a valid state
    for (const auto& move : moves) {
        auto newState = (*move)(board);
        ASSERT_TRUE(newState.has_value());
        EXPECT_TRUE(CheckTerminal::kingIsChecked(newState.value(), true).empty());
    }
}

TEST(GetAllMoves, InitialPositionBlack) {
    ChessBoard board;
    auto moves = getAllMoves(board, false, true);
    EXPECT_EQ(moves.size(), 20);
    EXPECT_EQ(CountMoveType<KnightsMove>(moves), 4);

    // ensure all moves lead to a valid state
    for (const auto& move : moves) {
        auto newState = (*move)(board);
        ASSERT_TRUE(newState.has_value());
        EXPECT_TRUE(CheckTerminal::kingIsChecked(newState.value(), true).empty());
    }
}

TEST(GetAllMoves, EmptyBoard) {
    ChessBoard board(
        std::string(64, static_cast<char>(ChessPiece::NONE)),
        Coord2D('e', 1), Coord2D('e', 8),
        false, false, false, false, std::nullopt, std::nullopt
    );
    auto moves = getAllMoves(board, true, true);
    EXPECT_EQ(moves.size(), 0);

    // ensure all moves lead to a valid state
    for (const auto& move : moves) {
        auto newState = (*move)(board);
        ASSERT_TRUE(newState.has_value());
        EXPECT_TRUE(CheckTerminal::kingIsChecked(newState.value(), true).empty());
    }
}

TEST(GetAllMoves, OnlyKing) {
    ChessBoard board(
        std::string(64, static_cast<char>(ChessPiece::NONE)),
        Coord2D('e', 1), Coord2D('e', 8),
        false, false, false, false, std::nullopt, std::nullopt
    );
    // Place white king at E1
    std::string raw = board.board();
    raw[Coord2D('e', 1).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_KING);
    ChessBoard kingBoard(
        raw, Coord2D('e', 1), Coord2D('e', 8),
        false, false, false, false, std::nullopt, std::nullopt
    );
    auto moves = getAllMoves(kingBoard, true, true);
    // King at E1 has 5 possible moves (D1, D2, E2, F1, F2) but only those on board
    EXPECT_EQ(moves.size(), 5);
    EXPECT_EQ(CountMoveType<QueensMove>(moves), 5);
    
    // ensure all moves lead to a valid state
    for (const auto& move : moves) {
        auto newState = (*move)(kingBoard);
        ASSERT_TRUE(newState.has_value()) << board.getWhitePOV();
        EXPECT_TRUE(CheckTerminal::kingIsChecked(newState.value(), true).empty());
    }
}

TEST(GetAllMoves, PawnPromotionMoves) {
    std::string raw(64, static_cast<char>(ChessPiece::NONE));
    raw[Coord2D('a', 7).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    ChessBoard board(
        raw, Coord2D('e', 1), Coord2D('e', 8),
        false, false, false, false, std::nullopt, std::nullopt
    );
    auto moves = getAllMoves(board, true, false);
    // Pawn at A7 can move to A8 (promotion), or capture at B8 if enemy present
    EXPECT_GE(moves.size(), 1);

    raw[Coord2D('b', 8).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_KNIGHT);
    ChessBoard boardWithCapture(
        raw, Coord2D('e', 1), Coord2D('e', 8),
        false, false, false, false, std::nullopt, std::nullopt
    );
    auto captureMoves = getAllMoves(boardWithCapture, true, false);
    // Now pawn can capture at B8
    EXPECT_GE(captureMoves.size(), 2);
}

TEST(GetAllMoves, BlockedPawn) {
    std::string raw(64, static_cast<char>(ChessPiece::NONE));
    raw[Coord2D('a', 2).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    raw[Coord2D('a', 3).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    ChessBoard board(
        raw, Coord2D('e', 1), Coord2D('e', 8),
        false, false, false, false, std::nullopt, std::nullopt
    );
    auto moves = getAllMoves(board, true, false);
    // Pawn is blocked, should have no moves
    EXPECT_EQ(moves.size(), 0) << board.getWhitePOV();
}

TEST(GetAllMoves, PawnEnPassant) {
    std::string raw(64, static_cast<char>(ChessPiece::NONE));
    raw[Coord2D('e', 4).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    raw[Coord2D('d', 4).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    ChessBoard board(
        raw, Coord2D('e', 1), Coord2D('e', 8),
        false, false, false, false, std::nullopt, Coord2D('e', 3)
    );
    auto moves = getAllMoves(board, false, false);
    // Pawn at E4 can capture en passant at D3
    EXPECT_EQ(moves.size(), 2);
    EXPECT_EQ(CountMoveType<QueensMove>(moves), 2);
}

TEST(GetAllMoves, KnightJumpOverPieces) {
    std::string raw(64, static_cast<char>(ChessPiece::NONE));
    raw[Coord2D('b', 1).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_KNIGHT);
    raw[Coord2D('b', 2).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    raw[Coord2D('c', 3).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_PAWN);
    ChessBoard board(
        raw, Coord2D('e', 1), Coord2D('e', 8),
        false, false, false, false, std::nullopt, std::nullopt
    );
    auto moves = getAllMoves(board, true, false);
    // Knight at B1 should have 2 moves: A3, C3 (C3 is a capture)
    EXPECT_EQ(moves.size(), 6) << board.getWhitePOV();
    EXPECT_EQ(CountMoveType<KnightsMove>(moves), 3);
}

TEST(GetAllMoves, RookMoves) {
    std::string raw(64, static_cast<char>(ChessPiece::NONE));
    raw[Coord2D('d', 4).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_ROOK);
    ChessBoard board(
        raw, Coord2D('e', 1), Coord2D('e', 8),
        false, false, false, false, std::nullopt, std::nullopt
    );
    auto moves = getAllMoves(board, true, false);
    // Rook at D4 should have 14 moves (7 up, 7 down, 3 left, 4 right)
    EXPECT_EQ(moves.size(), 14);
}

TEST(GetAllMoves, BishopMoves) {
    std::string raw(64, static_cast<char>(ChessPiece::NONE));
    raw[Coord2D('c', 1).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_BISHOP);
    ChessBoard board(
        raw, Coord2D('e', 1), Coord2D('e', 8),
        false, false, false, false, std::nullopt, std::nullopt
    );
    auto moves = getAllMoves(board, true, false);
    // Bishop at C1 should have 7 moves (diagonals)
    EXPECT_EQ(moves.size(), 7);
}

TEST(GetAllMoves, QueenMoves) {
    std::string raw(64, static_cast<char>(ChessPiece::NONE));
    raw[Coord2D('d', 4).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_QUEEN);
    ChessBoard board(
        raw, Coord2D('e', 1), Coord2D('e', 8),
        false, false, false, false, std::nullopt, std::nullopt
    );
    auto moves = getAllMoves(board, true, false);
    // Queen at D4 should have 27 moves (rook + bishop moves)
    EXPECT_EQ(moves.size(), 27);
}
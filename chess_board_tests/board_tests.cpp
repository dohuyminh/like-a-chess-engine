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

// Helper to create a board with only two pieces
ChessBoard boardWithTwo(Coord2D attacker, ChessPiece attackerPiece, Coord2D king, ChessPiece kingPiece) {
    std::string raw(64, static_cast<char>(ChessPiece::NONE));
    raw[attacker.toFlatIdx()] = static_cast<char>(attackerPiece);
    raw[king.toFlatIdx()] = static_cast<char>(kingPiece);
    return ChessBoard(raw, true, true, true, true, std::nullopt, std::nullopt);
}

TEST(KingIsCheckedTest, NoImmediateCapture) {
    // Only king on board
    std::string raw(64, static_cast<char>(ChessPiece::NONE));
    raw[Coord2D('E', 1).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_KING);
    ChessBoard board(raw, true, true, true, true, std::nullopt, std::nullopt);
    auto res = board.kingIsChecked(true);
    EXPECT_TRUE(res.empty());

    // King and friendly piece
    raw[Coord2D('D', 1).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_ROOK);
    ChessBoard board2(raw, true, true, true, true, std::nullopt, std::nullopt);
    res = board2.kingIsChecked(true);
    EXPECT_TRUE(res.empty());
}

TEST(KingIsCheckedTest, PawnCapturesKing) {
    // White pawn captures black king (diagonal)
    ChessBoard board1 = boardWithTwo(Coord2D('D', 7), ChessPiece::WHITE_PAWN, Coord2D('E', 8), ChessPiece::BLACK_KING);
    auto res = board1.kingIsChecked(false);
    EXPECT_EQ(res.size(), 1);
    EXPECT_TRUE(res.count(Coord2D('D', 7)));

    // Black pawn captures white king (diagonal)
    ChessBoard board2 = boardWithTwo(Coord2D('E', 2), ChessPiece::BLACK_PAWN, Coord2D('D', 1), ChessPiece::WHITE_KING);
    res = board2.kingIsChecked(true);
    EXPECT_EQ(res.size(), 1);
    EXPECT_TRUE(res.count(Coord2D('E', 2)));

    // Pawn cannot capture king straight
    ChessBoard board3 = boardWithTwo(Coord2D('E', 2), ChessPiece::BLACK_PAWN, Coord2D('E', 1), ChessPiece::WHITE_KING);
    res = board3.kingIsChecked(true);
    EXPECT_TRUE(res.empty());

    // Pawn at edge only captures one way
    ChessBoard board4 = boardWithTwo(Coord2D('A', 2), ChessPiece::BLACK_PAWN, Coord2D('B', 1), ChessPiece::WHITE_KING);
    res = board4.kingIsChecked(true);
    EXPECT_EQ(res.size(), 1);
    EXPECT_TRUE(res.count(Coord2D('A', 2)));
}

TEST(KingIsCheckedTest, KnightCapturesKing) {
    // White knight captures black king
    ChessBoard board1 = boardWithTwo(Coord2D('C', 3), ChessPiece::WHITE_KNIGHT, Coord2D('D', 5), ChessPiece::BLACK_KING);
    auto res = board1.kingIsChecked(false);
    EXPECT_EQ(res.size(), 1);
    EXPECT_TRUE(res.count(Coord2D('C', 3)));

    // Black knight captures white king
    ChessBoard board2 = boardWithTwo(Coord2D('F', 3), ChessPiece::BLACK_KNIGHT, Coord2D('E', 1), ChessPiece::WHITE_KING);
    res = board2.kingIsChecked(true);
    EXPECT_EQ(res.size(), 1);
    EXPECT_TRUE(res.count(Coord2D('F', 3)));

    // Knight not in range
    ChessBoard board3 = boardWithTwo(Coord2D('C', 3), ChessPiece::WHITE_KNIGHT, Coord2D('C', 6), ChessPiece::BLACK_KING);
    res = board3.kingIsChecked(false);
    EXPECT_TRUE(res.empty());
}

TEST(KingIsCheckedTest, BishopCapturesKing) {
    // White bishop captures black king (up-right)
    ChessBoard board1 = boardWithTwo(Coord2D('C', 1), ChessPiece::WHITE_BISHOP, Coord2D('F', 4), ChessPiece::BLACK_KING);
    auto res = board1.kingIsChecked(false);
    EXPECT_EQ(res.size(), 1);
    EXPECT_TRUE(res.count(Coord2D('C', 1)));

    // Black bishop captures white king (down-left)
    ChessBoard board2 = boardWithTwo(Coord2D('F', 4), ChessPiece::BLACK_BISHOP, Coord2D('C', 1), ChessPiece::WHITE_KING);
    res = board2.kingIsChecked(true);
    EXPECT_EQ(res.size(), 1);
    EXPECT_TRUE(res.count(Coord2D('F', 4)));

    // Bishop blocked by piece
    std::string raw(64, static_cast<char>(ChessPiece::NONE));
    raw[Coord2D('C', 1).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_BISHOP);
    raw[Coord2D('D', 2).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    raw[Coord2D('F', 4).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_KING);
    ChessBoard board3(raw, true, true, true, true, std::nullopt, std::nullopt);
    res = board3.kingIsChecked(false);
    EXPECT_TRUE(res.empty());
}

TEST(KingIsCheckedTest, RookCapturesKing) {
    // White rook captures black king (horizontal)
    ChessBoard board1 = boardWithTwo(Coord2D('A', 1), ChessPiece::WHITE_ROOK, Coord2D('H', 1), ChessPiece::BLACK_KING);
    auto res = board1.kingIsChecked(false);
    EXPECT_EQ(res.size(), 1);
    EXPECT_TRUE(res.count(Coord2D('A', 1)));

    // Black rook captures white king (vertical)
    ChessBoard board2 = boardWithTwo(Coord2D('D', 8), ChessPiece::BLACK_ROOK, Coord2D('D', 1), ChessPiece::WHITE_KING);
    res = board2.kingIsChecked(true);
    EXPECT_EQ(res.size(), 1);
    EXPECT_TRUE(res.count(Coord2D('D', 8)));

    // Rook blocked by piece
    std::string raw(64, static_cast<char>(ChessPiece::NONE));
    raw[Coord2D('A', 1).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_ROOK);
    raw[Coord2D('D', 1).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    raw[Coord2D('H', 1).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_KING);
    ChessBoard board3(raw, true, true, true, true, std::nullopt, std::nullopt);
    res = board3.kingIsChecked(false);
    EXPECT_TRUE(res.empty());
}

TEST(KingIsCheckedTest, QueenCapturesKing) {
    // White queen captures black king (diagonal)
    ChessBoard board1 = boardWithTwo(Coord2D('A', 1), ChessPiece::WHITE_QUEEN, Coord2D('H', 8), ChessPiece::BLACK_KING);
    auto res = board1.kingIsChecked(false);
    EXPECT_EQ(res.size(), 1);
    EXPECT_TRUE(res.count(Coord2D('A', 1)));

    // Black queen captures white king (vertical)
    ChessBoard board2 = boardWithTwo(Coord2D('D', 8), ChessPiece::BLACK_QUEEN, Coord2D('D', 1), ChessPiece::WHITE_KING);
    res = board2.kingIsChecked(true);
    EXPECT_EQ(res.size(), 1);
    EXPECT_TRUE(res.count(Coord2D('D', 8)));

    // Queen blocked by piece
    std::string raw(64, static_cast<char>(ChessPiece::NONE));
    raw[Coord2D('A', 1).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_QUEEN);
    raw[Coord2D('D', 4).toFlatIdx()] = static_cast<char>(ChessPiece::WHITE_PAWN);
    raw[Coord2D('H', 8).toFlatIdx()] = static_cast<char>(ChessPiece::BLACK_KING);
    ChessBoard board3(raw, true, true, true, true, std::nullopt, std::nullopt);
    res = board3.kingIsChecked(false);
    EXPECT_TRUE(res.empty());
}

TEST(KingIsCheckedTest, KingCapturesKing) {
    // White king next to black king
    ChessBoard board1 = boardWithTwo(Coord2D('E', 2), ChessPiece::WHITE_KING, Coord2D('E', 1), ChessPiece::BLACK_KING);
    auto res = board1.kingIsChecked(false);
    EXPECT_EQ(res.size(), 1);
    EXPECT_TRUE(res.count(Coord2D('E', 2)));

    // Black king next to white king
    ChessBoard board2 = boardWithTwo(Coord2D('E', 2), ChessPiece::BLACK_KING, Coord2D('E', 1), ChessPiece::WHITE_KING);
    res = board2.kingIsChecked(true);
    EXPECT_EQ(res.size(), 1);
    EXPECT_TRUE(res.count(Coord2D('E', 2)));

    // Kings not adjacent
    ChessBoard board3 = boardWithTwo(Coord2D('E', 3), ChessPiece::WHITE_KING, Coord2D('E', 1), ChessPiece::BLACK_KING);
    res = board3.kingIsChecked(false);
    EXPECT_TRUE(res.empty());
}
#include "fen.h"
#include "check_terminal/checkmate.h"
#include "coord2D.h"
#include "utility.h"

#include <cctype>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace internal {

State::State(
    const ChessBoard& board, 
    Color turn, 
    std::size_t movesWithoutProgress,
    LRULegalMoveGen& generator 
) : board(board), turn(turn), movesWithoutProgress(movesWithoutProgress), generator(generator), isTerminal(false), winner(Color::NONE) {
    
    const std::vector< std::shared_ptr < ChessMove > >& moves = generator.get(board, turn); 

    // determine if the board is terminal
    CheckTerminal::MateStatus ms = CheckTerminal::isCheckmate(board, turn, moves.size());

    // if the current player is mated, it's terminal
    if (ms == CheckTerminal::MateStatus::CHECKMATE) {
        isTerminal = true;
        winner = ~turn;
    }

    // stalemate 
    else if (moves.empty()) {
        isTerminal = true;
    }

    // FIDE specifies after 75 moves without progress it's an automatic draw
    else if (movesWithoutProgress >= 75) {
        isTerminal = true;
    }

    // TODO: Implement dead position detection 
}

FENParsingException::FENParsingException(const char* msg) : _msg(msg) {}

FENParsingException::FENParsingException(const char* msg, std::string fen, int idx) {
    _msg = msg;
    _msg.push_back('\n');

    _msg += fen;
    _msg.push_back('\n');

    std::string arr(fen.size(), ' ');
    arr[idx] = '^';
    _msg += arr;
}

std::unordered_map< char, ChessPiece > FENlookup = {
    { 'P', ChessPiece::WHITE_PAWN   }, 
    { 'R', ChessPiece::WHITE_ROOK   }, 
    { 'N', ChessPiece::WHITE_KNIGHT }, 
    { 'B', ChessPiece::WHITE_BISHOP }, 
    { 'Q', ChessPiece::WHITE_QUEEN  }, 
    { 'K', ChessPiece::WHITE_KING   },
    { 'p', ChessPiece::BLACK_PAWN   }, 
    { 'r', ChessPiece::BLACK_ROOK   }, 
    { 'n', ChessPiece::BLACK_KNIGHT }, 
    { 'b', ChessPiece::BLACK_BISHOP }, 
    { 'q', ChessPiece::BLACK_QUEEN  }, 
    { 'k', ChessPiece::BLACK_KING   },
};

std::size_t parsePiecePlacement(const std::string& fen, std::size_t idx, char* boardData) {
    // extract the piece placement     
    std::size_t bound = std::string::npos;

    if ((bound = fen.find(' ')) == std::string::npos) {
        throw FENParsingException("FEN parsing error; cannot find piece placement string");
    }
    
    char col = 'a';
    int8_t row = 8;

    // keeps track of king placements and column shifting 
    bool lastTokIsNum = false, whiteKingPlaced = false, blackKingPlaced = false;

    // iterate through each character and place pieces accordingly
    for (; idx < bound; ++idx) {
        
        char c = fen[idx];
    
        // found a '/' -> move to the row below
        if (c == '/') {
            // if the current row is not finished processing
            if (col != 'h' + 1) {
                throw FENParsingException("FEN parsing error; current row not finished processing", fen, idx);
            }

            // if the current row is the last one         
            else if (row == 1) {
                throw FENParsingException("FEN parsing error; cannot shift down from bottom row", fen, idx);
            }

            // otherwise, shift to the row below
            --row;
            col = 'a';
            lastTokIsNum = false;
        }

        // found a digit -> shift columns to the rights that many times
        else if (std::isdigit(c)) {
            
            // a column shifting cannot be followed by a column shifting 
            if (lastTokIsNum) {
                throw FENParsingException("FEN parsing error; column shifting cannot be followed by another column shifting", fen, idx);
            }

            int colShift = c - '0';

            col += colShift;

            // columns out of bounds
            if (col > 'h' + 1) {
                throw FENParsingException("FEN parsing error; column shifting leads to out of bounds", fen, idx);
            }

            lastTokIsNum = true;
        } 

        // piece placement 
        else if (FENlookup.count(c)) {
            
            // verify if we're not at the edge of the board (supposed to get to shift row but instead did a placement) 
            if (col >= 'h' + 1) {
                throw FENParsingException("FEN parsing error; placing piece out of board", fen, idx);
            } 

            // verify if a king has been placed
            ChessPiece piece = FENlookup.at(c);
            if (piece == ChessPiece::WHITE_KING) {
                if (whiteKingPlaced) {
                    throw FENParsingException("FEN parsing error; multiple white king placements on board", fen, idx);
                } 
                whiteKingPlaced = true;
            }
            else if (piece == ChessPiece::BLACK_KING) {
                if (blackKingPlaced) {
                    throw FENParsingException("FEN parsing error; multiple black king placements on board", fen, idx);
                } 
                blackKingPlaced = true;               
            }
            
            // place piece on board
            utility::writeData(boardData, Coord2D(col, row), FENlookup.at(c));            
            
            ++col;
            lastTokIsNum = false;
        }  

        // unrecognized token
        else {
            throw FENParsingException("FEN parsing error; unrecognized token at piece placement", fen, idx);
        }
    } 

    // in the end, all the board should be scanned through
    if (col != 'h' + 1 || row != 1) {
        throw FENParsingException("FEN parsing error; piece placement did not cover all the squares in the board");
    }
    // and both white and black kings are inserted
    if (!(whiteKingPlaced && blackKingPlaced)) {
        throw FENParsingException("FEN parsing error; either the white king or the black king has not been placed");
    }

    return bound + 1;
}

std::size_t parseActivePlayer(const std::string& fen, std::size_t idx, Color& activePlayer) {
    
    std::size_t bound = std::string::npos;

    if ((bound = fen.find(' ', idx)) == std::string::npos) {
        throw FENParsingException("FEN parsing error; cannot find active player");
    }

    // active player may only be white ("w") or black ("b") 
    if (bound - idx > 1 || (fen[idx] != 'w' && fen[idx] != 'b')) {
        throw FENParsingException("FEN parsing error; unrecognized token in active player", fen, idx);
    }

    activePlayer = (fen[idx] == 'w') ? Color::WHITE : Color::BLACK;

    return bound + 1;
}

std::size_t parseCastling(const std::string& fen, std::size_t idx, char* boardData) {
    
    std::size_t bound = std::string::npos;
    if ((bound = fen.find(' ', idx)) == std::string::npos) {
        throw FENParsingException("FEN parsing error; cannot find castling rights");
    }
    
    // if not specified -> turn off all castling rights
    if (bound - idx == 1 && fen[idx] == '-') {
        boardData[32] = 0;
    }
    // if specified 
    else if (bound - idx <= 4) {
         
        //               K  Q  k  q
        bool seen[4] = { 0, 0, 0, 0 };

        for (; idx < bound; ++idx) {
            
            ChessPiece piece = ChessPiece::NONE; 
            Coord2D rookPos('a', 1);
            std::size_t rIdx = 0;

            ChessPiece king = ChessPiece::NONE;

            switch (fen[idx]) {
                case 'K':
                    piece = ChessPiece::WHITE_ROOK;
                    rookPos = Coord2D('a', 1);
                    rIdx = 1;
                    king = ChessPiece::WHITE_KING;
                    break;
                case 'Q':
                    piece = ChessPiece::WHITE_ROOK;
                    rookPos = Coord2D('h', 1);
                    rIdx = 0;
                    king = ChessPiece::WHITE_KING;
                    break;
                case 'k':
                    piece = ChessPiece::BLACK_ROOK;
                    rookPos = Coord2D('h', 8);
                    rIdx = 3;
                    king = ChessPiece::BLACK_KING;
                    break;
                case 'q':
                    piece = ChessPiece::BLACK_ROOK;
                    rookPos = Coord2D('a', 8);
                    rIdx = 2;
                    king = ChessPiece::BLACK_KING;
                    break;
                default:
                    throw FENParsingException("FEN parsing error; unrecognizable token at castling rights", fen, idx);
            }

            // verify if the rook has moved/been captured
            if (utility::getData(boardData, rookPos) != piece) {
                throw FENParsingException("FEN parsing error; castling rights for a rook that's moved/captured", fen, idx);
            }
            // rights repetition 
            else if (seen[rIdx]) {
                throw FENParsingException("FEN parsing error; repetition of castling rights", fen, idx);
            }
            else if ((king == ChessPiece::WHITE_KING && utility::getData(boardData, Coord2D('e', 1)) != king) || 
                    (king == ChessPiece::BLACK_KING && utility::getData(boardData, Coord2D('e', 8)) != king)) {

                throw FENParsingException("FEN parsing error; castling rights for a king that's moved", fen, idx);
            }

            // set the castling rights to true
            boardData[32] |= 1 << rIdx;           
        }

    }
    // unrecognized string 
    else {
        throw FENParsingException("FEN parsing error; unrecognizable castling rights", fen, idx);
    }

    return bound + 1;    
}

std::size_t parseEnPassant(const std::string& fen, std::size_t idx, Color activePlayer, char* boardData) {
    
    std::size_t bound = std::string::npos;
    if ((bound = fen.find(' ', idx)) == std::string::npos) {
        throw FENParsingException("FEN parsing error; cannot find en passant");
    }

    // en passant section can only be a valid square or empty ("-")
    if (bound - idx == 1 && fen[idx] == '-') {
        // no en passant 
        boardData[33] = 0;
    } else if (bound - idx == 2) {
        // en passant; verify if it's a valid square 
        if (fen[idx] < 'a' || fen[idx] > 'h') {
            throw FENParsingException("FEN parsing error; unrecognized token in en passant", fen, idx);
        } 

        int8_t row = fen[idx + 1] - '0';

        // row = 3 -> white pawn en passant 
        if (row == 3) {

            // makes no sense if the active player is white now
            if (activePlayer == Color::WHITE) {
                throw FENParsingException("FEN parsing error; it's white turn while the en passant is targetting a white pawn");
            }

            // verify if there was actually a pawn 
            if (utility::getData(boardData, Coord2D(fen[idx], row + 1)) != ChessPiece::WHITE_PAWN) {
                throw FENParsingException("FEN parsing error; en passant square without presence of a pawn", fen, idx + 1);
            }

            // insert en passant data 
            utility::setEnPassant(boardData, Color::BLACK, Coord2D(fen[idx], row));
        
        }
        // row = 6 -> black pawn en passant
        else if (row == 6) {
            
            if (activePlayer == Color::BLACK) {
                throw FENParsingException("FEN parsing error; it's black turn while the en passant is targetting a black pawn");
            }

            if (utility::getData(boardData, Coord2D(fen[idx], row - 1)) != ChessPiece::BLACK_PAWN) {
                throw FENParsingException("FEN parsing error; en passant square without presence of a pawn", fen, idx + 1);
            }

            // insert en passant data 
            utility::setEnPassant(boardData, Color::WHITE, Coord2D(fen[idx], row));

        }
        // invalid square for en passant 
        else {
            throw FENParsingException("FEN parsing error; invalid en passant square", fen, idx);
        }
    } else {
        throw FENParsingException("FEN parsing error; abnormally long en passant string"); 
    }

    return bound + 1;
}

std::size_t parseHalfMoves(const std::string& fen, std::size_t idx, std::size_t& halfMoves) {
       
    std::size_t bound = std::string::npos;
    if ((bound = fen.find(' ', idx)) == std::string::npos) {
        throw FENParsingException("FEN parsing error; cannot find half clock");
    }

    // verify if the string is fully numeric
    for (std::size_t i = idx; i < bound; ++i) {
        if (!std::isdigit(fen[i])) {
            throw FENParsingException("FEN parsing error; non-digit token found in half clock", fen, idx);
        }
    } 

    halfMoves = std::stoull(fen.substr(idx, bound - idx));    

    return bound + 1; 
}

std::size_t parseFullMoves(const std::string& fen, std::size_t idx, std::size_t& fullMoves) {
    
    std::size_t bound = fen.size();

    // verify if the string is fully numeric
    for (std::size_t i = idx; i < bound; ++i) {
        if (!std::isdigit(fen[i])) {
            throw FENParsingException("FEN parsing error; non-digit token found in full moves", fen, idx);
        }
    } 

    fullMoves = std::stoull(fen.substr(idx, bound - idx));    
    if (fullMoves < 1) {
        throw FENParsingException("FEN parsing error; full moves must start with 1");
    }

    return bound;
}

State loadStateFromFen(std::string fen, LRULegalMoveGen& generator) {
    
    std ::size_t idx = 0;

    
    char boardData[34] = { 0 };

    idx = parsePiecePlacement(fen, idx, boardData);

    Color activePlayer = Color::NONE;
    idx = parseActivePlayer(fen, idx, activePlayer);

    idx = parseCastling(fen, idx, boardData);

    idx = parseEnPassant(fen, idx, activePlayer, boardData);

    std::size_t halfMoves = (std::size_t)-1;
    idx = parseHalfMoves(fen, idx, halfMoves);

    std::size_t fullMoves = (std::size_t)-1;
    idx = parseFullMoves(fen, idx, fullMoves);

    // get the current board and determinee if it's terminal
    ChessBoard board(boardData);    

    // return full state 
    return State(board, activePlayer, halfMoves, generator); 
}

}

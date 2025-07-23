#include <iostream>
#include "chess_board.h"
#include "move.h"
#include <deque>

inline bool isValidCoord(const std::string& cmd) {
    if (cmd.size() != 2) return false;
    if (cmd[0] < 'A' || cmd[0] > 'H') return false;
    if (cmd[1] < '1' || cmd[1] > '8') return false;
    return true;
}

inline bool isValidDirection(const std::string& cmd) {
    return !(cmd != "UP" &&
            cmd != "DOWN" &&
            cmd != "LEFT" &&
            cmd != "RIGHT" &&
            cmd != "UP LEFT" &&
            cmd != "UP RIGHT" &&
            cmd != "DOWN LEFT" &&
            cmd != "DOWN RIGHT");
}

QueensMove constructQueensMove(bool isWhiteTurn) {

    std::string cmd;
    bool valid = true;

    std::cout << "Choose which piece to move [<A-H><1-8>]\n: ";

    // choose origin
    do {
        std::getline(std::cin, cmd);
        valid = isValidCoord(cmd);
        if (!valid) {
            std::cout << "Invalid input, try again\n: ";
        }
    } while (!valid);
    const char col = cmd[0];
    const int8_t row = cmd[1] - '0';
    const Coord2D origin(col, row);

    // choose direction
    std::cout << "Choose the direction [UP/DOWN/LEFT/RIGHT/UP LEFT/UP RIGHT/DOWN LEFT/DOWN RIGHT]\n: ";
    do {
        std::getline(std::cin, cmd);
        valid = isValidDirection(cmd);
        if (!valid) {
            std::cout << "Invalid input, try again\n: ";
        }
    } while (!valid);

    Direction dir;
    if (cmd == "UP") dir = Direction::UP;
    else if (cmd == "DOWN") dir = Direction::DOWN;
    else if (cmd == "LEFT") dir = Direction::LEFT;
    else if (cmd == "RIGHT") dir = Direction::RIGHT;
    else if (cmd == "UP LEFT") dir = Direction::UP_LEFT;
    else if (cmd == "UP RIGHT") dir = Direction::UP_RIGHT;
    else if (cmd == "DOWN LEFT") dir = Direction::DOWN_LEFT;
    else dir = Direction::DOWN_RIGHT;

    // choose magnitude
    std::cout << "Determine how far the piece will go in the board [1-7]\n: ";
    uint8_t magnitude = 0;
    do {
        std::getline(std::cin, cmd);
        magnitude = std::stoi(cmd);
        if (magnitude > 7) {
            std::cout << "Invalid input, try again\n: ";
        }
    } while (magnitude > 7);

    // create transformation function
    return QueensMove(isWhiteTurn, dir, magnitude, origin);
}

KnightsMove constructKnightsMove(bool turnIsWhite) {

    std::string cmd;
    bool valid = true;

    // get origin piece
    std::cout << "Choose which piece to move [<A-H><1-8>]\n: ";
    do {
        std::getline(std::cin, cmd);
        valid = isValidCoord(cmd);
        if (!valid) {
            std::cout << "Invalid input, try again\n: ";
        }
    } while (!valid);

    const char col = cmd[0];
    const int8_t row = cmd[1] - '0';
    const Coord2D origin(col, row);

    Vec2D mvSet[8] = {
        Vec2D(-2, -1),
        Vec2D(-1, -2),
        Vec2D(1, -2),
        Vec2D(2, -1),
        Vec2D(-2, 1),
        Vec2D(-1, 2),
        Vec2D(2, 1),
        Vec2D(1, 2)
    };

    std::cout << "Choose the knight's direction:\n";
    std::cout << "\t1. <-2, -1>\n";
    std::cout << "\t2. <-1, -2>\n";
    std::cout << "\t3. <1, -2>\n";
    std::cout << "\t4. <2, -1>\n";
    std::cout << "\t5. <-2, 1>\n";
    std::cout << "\t6. <-1, 2>\n";
    std::cout << "\t7. <2, 1>\n";
    std::cout << "\t8. <1, 2>\n: ";

    int idx;
    do {
        std::getline(std::cin, cmd);
        idx = std::stoi(cmd);
        valid = 1 <= idx && idx <= 8;
        if (!valid) {
            std::cout << "Invalid input, try again\n: ";
        }
    } while (!valid);

    Vec2D mv = mvSet[idx - 1];

    return KnightsMove(turnIsWhite, mv, origin);
}

int main() {

    std::deque<ChessBoard> trace;
    trace.emplace_back();

    bool isWhiteTurn = true;

    std::cout << "Let's play a game of chess at 2AM lol\n";

    while (true) {
        const ChessBoard& currState = trace.back();

        if (isWhiteTurn) {
            std::cout << "-----WHITE'S TURN-----\n\n" << trace.back().getWhitePOV();
            std::cout << "Left Castling: " << (currState.whiteLeftCastling() ? "true" : "false") << '\n';
            std::cout << "Right Castling: " << (currState.whiteRightCastling() ? "true" : "false") << '\n';
            std::cout << "Black Enpassant: ";
            if (!currState.blackEnpassant().has_value()) {
                std::cout << "None\n";
            } else {
                std::string coord;
                coord.push_back(currState.blackEnpassant()->col());
                coord.push_back(static_cast<char>('0' + currState.blackEnpassant()->row()));
                std::cout << coord << '\n';
            }
        }
        else {
            std::cout << "-----BLACK'S TURN-----\n\n" << trace.back().getBlackPOV();
            std::cout << "Left Castling: " << (currState.blackLeftCastling() ? "true" : "false") << '\n';
            std::cout << "Right Castling: " << (currState.blackRightCastling() ? "true" : "false") << '\n';
            std::cout << "White Enpassant: ";
            if (!currState.whiteEnpassant().has_value()) {
                std::cout << "None\n";
            }
            else {
                std::string coord;
                coord.push_back(currState.whiteEnpassant()->col());
                coord.push_back(static_cast<char>('0' + currState.whiteEnpassant()->row()));
                std::cout << coord << '\n';
            }
        }

        std::cout << "Choose action:\n\t- Play a move [P]\n\t- Undo action [U]\n\t- Forfeit [X]\n: ";
        std::string cmd;

        do {
            std::getline(std::cin, cmd);
            if (cmd != "P" && cmd != "X" && cmd != "U" && cmd != "Q") {
                std::cout << "Invalid input, try again\n: ";
            }
            else if (cmd == "U" && trace.size() == 1) {
                std::cout << "Undo may not be done, try again\n: ";
                cmd.clear();
            }
            else if (cmd == "Q") {
                std::cout << "Which piece?\n: ";
                do {
                    std::getline(std::cin, cmd);
                    if (!isValidCoord(cmd)) {
                        std::cout << "Invalid input, try again\n: ";
                    }
                } while (!isValidCoord(cmd));
                const char col = cmd[0];
                const int8_t row = cmd[1] - '0';
                const Coord2D origin(col, row);
                std::cout << "The piece at requested coordinate is: " << (int)currState.getPiece(origin) << '\n';
                std::cout << ": ";
            }
        } while (cmd != "P" && cmd != "X" && cmd != "U");

        if (cmd == "X") {
            std::cout << "Player has forfeit\n";
            break;
        }
        if (cmd == "U") {
            trace.pop_front();
            isWhiteTurn = !isWhiteTurn;
            continue;
        }

        std::cout << "Choose a move:\n";
        std::cout << "\t- Queen's move [Q]\n";
        std::cout << "\t- Knight's move [K]\n";
        std::cout << "\t- Castling [C]\n";
        std::cout << "\t- Underpromotion [U]\n";
        std::cout << ": ";
        do {
            std::getline(std::cin, cmd);
            if (cmd != "Q" && cmd != "K" && cmd != "C" && cmd != "U") {
                std::cout << "Invalid input, try again\n: ";
            }
        } while (cmd != "Q" && cmd != "K" && cmd != "C" && cmd != "U");

        if (cmd == "Q") {
            std::optional<ChessBoard> nextState = constructQueensMove(isWhiteTurn)(currState);
            if (!nextState.has_value()) {
                std::cout << "FATAL: EXIT NOW\n";
                return -1;
            }
            trace.push_back(*nextState);
        } else if (cmd == "K") {
            std::optional<ChessBoard> nextState = constructKnightsMove(isWhiteTurn)(currState);
            if (!nextState.has_value()) {
                std::cout << "FATAL: EXIT NOW\n";
                return -1;
            }
            trace.push_back(*nextState);
        } else if (cmd == "C") {

        } else {

        }

        isWhiteTurn = !isWhiteTurn;
    }

    return 0;
}

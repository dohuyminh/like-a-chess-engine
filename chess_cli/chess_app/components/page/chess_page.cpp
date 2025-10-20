#include "chess_page.h"

#include <curses.h>

inline static int centerMsg(std::size_t msgLen, std::size_t windowWidth) {
    return (msgLen >= windowWidth) ? 0 : (windowWidth - msgLen) >> 1;
} 

// ChessPage
ChessPage::ChessPage(std::size_t pageIdx, std::shared_ptr<PageControlBlock> pcb, std::string fen) : 
    Page(pageIdx, pcb), _arena(fen) {}

void ChessPage::render() {
    for (const auto& [_, win] : _windows) {
        win->render();
    }
}

void ChessPage::performUpdate() {
    // Base implementation - can be enhanced later with game-specific updates
    for (const auto& [_, win] : _windows) {
        win->update();
    }
}
// ChessPage!

// ChessRootWindow
ChessRootWindow::ChessRootWindow(std::shared_ptr<Page> page) : 
    Window(page, 27, 0, 0, 0, 0) 
{}

void ChessRootWindow::render() const {

}

void ChessRootWindow::sendUpdateToPage(int input) {
    
    // receives 3 (^C) -> quit application
    if (input == 3) {
        _page->signalAppStop();
    }

}
// ChessRootWindow!

// BoardWindow
void BoardWindow::initColorSetting() {

    start_color();
    use_default_colors();

    init_pair(1, -1, 15);
    init_pair(2, -1, 22);

}

BoardWindow::BoardWindow(std::shared_ptr<Page> page, Color orientation) : 
    // Window dimensions:
    // Height = 8 squares * 3 lines + 4 for borders and labels = 28
    // Width = 8 squares * 5 chars + 4 for borders and labels = 44
    Window(page, 'b', 1, 1, 44, 28), _orientation(orientation) {

    if (_orientation == Color::NONE) {
        throw std::invalid_argument("BoardWindow; orientation can only either be White or Black");
    }

    // Initialize ncurses color pairs
    initColorSetting();
    
    // Ensure window has a fixed size
    wresize(_window, 28, 44);
}

void BoardWindow::render() const {

    // box the window
    box(_window, 0, 0);

    // name of the window
    mvwprintw(_window, 0, 2, "Board (b)");

    // print board status (player turn/winner)
    std::string msg;

    Arena& _arena = static_cast<ChessPage*>(_page.get())->arena();

    if (_arena.gameHasEnded()) {
        Color winner = _arena.winner();
        
        std::string w = (winner == Color::NONE) ? "it's a draw" : winner + " has won!";
        msg += "Game has ended; ";
        msg += w;

    } else {
        
        msg += _arena.turn() + " to move"; 

    }

    // print header message here 
    int place = centerMsg(msg.size(), _width);    
    mvwprintw(_window, 2, place, "%s", msg.c_str());

    // print board
    constexpr int SQUARE_HEIGHT = 3;
    constexpr int SQUARE_WIDTH  = 5;

    // Calculate starting position to center the board
    int rowPlacement = 2; // Leave space for border and rank numbers

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            // print each square individually
            char col = c + 'a';
            int  row = (_orientation == Color::WHITE) ? 8 - r : r + 1;

            Coord2D coord(col, row);

            // Print rank numbers on the left
            mvwprintw(_window, 4 + r * SQUARE_HEIGHT, rowPlacement, "%d", row);

            // square coloring goes here            
            int color = (r + c) & 1 ? 2 : 1;

            wattron(_window, COLOR_PAIR(color));
            for (int dy = 0; dy < SQUARE_HEIGHT; ++dy) {
                for (int dx = 0; dx < SQUARE_WIDTH; ++dx) {
                    mvwaddch(_window, 4 + r * SQUARE_HEIGHT + dy, 2 + rowPlacement + c * SQUARE_WIDTH + dx, ' ');
                }
            }
            wattroff(_window, COLOR_PAIR(color));

            ChessPiece piece = _arena.currentBoard().getPiece(coord);
            if (!piece.isNone()) {
                mvwprintw(_window, 4 + r * SQUARE_HEIGHT + 1, 2 + rowPlacement + c * SQUARE_WIDTH + 2, "%s", piece.toAscii());
            }
        }
    }

    // Print file labels (a-h) at the bottom
    for (int c = 0; c < 8; ++c) {
        mvwprintw(_window, 4 + 8 * SQUARE_HEIGHT, 4 + rowPlacement + c * SQUARE_WIDTH + 1, "%c", 'a' + c);
    }

    wrefresh(_window);
}

void BoardWindow::sendUpdateToPage(int input) {
    // board window does not handle any input
    return;
}
// BoardWindow!
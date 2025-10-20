#include "chess_app_builder.h"
#include "chess_page.h"

#include <cstdio>

ChessApp appInit() {
    
    // initialize PCB
    auto pcb = std::make_shared<PageControlBlock>(0);
    pcb->shiftWindow(27); // root window is active at start

    // initialize pages
    auto chessPage = std::make_shared<ChessPage>(0, pcb); 

    // initialize windows
    auto rootWindow  = std::make_unique<ChessRootWindow>(chessPage);
    auto boardWindow = std::make_unique<BoardWindow>(chessPage);
    
    // add windows to pages
    bool b1 = chessPage->addWindow(std::move( rootWindow));
    bool b2 = chessPage->addWindow(std::move(boardWindow));

    // initialize app
    Resources resources{ chessPage };

    return ChessApp(std::move(resources), pcb);

}
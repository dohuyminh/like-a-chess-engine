#include "chess_app.h"

ChessApp::ChessApp(Resources&& resources, std::shared_ptr<PageControlBlock> pcb) : 
    App(std::move(resources), pcb) {}
#pragma once

#include "app.h"

class ChessApp : public App {
public:

    ChessApp(Resources&& resources, std::shared_ptr<PageControlBlock> pcb);

};
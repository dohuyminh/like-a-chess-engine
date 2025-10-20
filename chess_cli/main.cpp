#include <iostream>

#include "chess_board.h"
#include "chess_app_builder.h"

int main(int argc, char const *argv[]) {
    
    ChessApp app = appInit();

    app.render();

    return 0;
}

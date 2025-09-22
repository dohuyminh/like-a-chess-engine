#include "window.h"

Window::Window(Page& page, int activateKey, int ty, int tx, int height, int width) :
    _page(page), _ty(ty), _tx(tx), _height(height), _width(width) 
{

    // reserve esc key for switching from current window to global screen (stdscr)    
    if (_activateKey == 27) {
        throw std::invalid_argument("Window; key 27 (ESC) is reserved for global window");
    }

}

void Window::update() {

    // wait for input 
    auto key = getInput();

    // if the window receives a key -> send update to page
    if (key.has_value()) {
        sendUpdateToPage(key.value());        
    }

    // otherwise, no update is made; keep rendering
}

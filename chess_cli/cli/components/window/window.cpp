#include "window.h"
#include <stdexcept>
#include <string>

Window::Window(std::shared_ptr< Page > page, int activateKey, int ty, int tx, int height, int width) :
    _page(page), _activateKey(activateKey), _ty(ty), _tx(tx), _height(height), _width(width) 
{

}

void Window::update() {

    // wait for input 
    auto key = getInput();

    // if the window receives a key -> send update to page
    if (key.has_value()) {
        sendUpdateToPage(key.value()); 
    }

    // otherwise, no update via input is made; keep rendering
}

Page::Page(std::size_t pageIdx, std::shared_ptr<PageControlBlock> pcb) : 
    _pageIdx(pageIdx), _pcb(pcb) {}

Page::~Page() {
    clean();  // Make sure to clean up windows in destructor
}

void Page::update(int currentWindow) {
    
    // ensure the window is in the page
    if (!_windows.count(currentWindow)) {
        throw std::runtime_error("Page; page " + std::to_string(_pageIdx) + " cannot find window of activation key " + std::to_string(currentWindow));
    }

    // receive update from window 
    _windows.at(currentWindow)->update();
}

bool Page::isInitialized() const noexcept {
    for (const auto& [_, win] : _windows) {
        if (!win->isInitialized()) {
            return false;
        }
    }
    return true;
}

void Page::initWindows() noexcept {
    for (const auto& [_, win] : _windows) {
        if (!win->isInitialized()) {
            win->init();
        }
    }
}

void Page::clean() noexcept {
    for (const auto& [_, win] : _windows) {
        if (win->isInitialized()) {
            win->free();
        }
    }
}

Window::~Window() {
    if (_window && _activateKey != 27) {
        delwin(_window);
        _window = nullptr;
    }
}

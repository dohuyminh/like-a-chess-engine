#include "app.h"

#include <curses.h>
#include <memory>
#include <stdexcept>
#include <iostream>

App::App(Resources&& resources, std::shared_ptr<PageControlBlock> pcb) : 
    _resources(std::move(resources)), _pcb(pcb) {}

void App::render() {
    
    // initialize curses
    if (!initscr()) {
        throw std::runtime_error("App; failed to initialize curses");
    }    

    clear();
    noecho();
    cbreak();
    raw();  // Disable all special character handling including signals

    while (!_pcb->appStopped()) {
        
        // select which page to render
        std::shared_ptr<Page> currPage = _resources.getPage(_pcb->currenntPageIdx());

        // initialize all windows in the page
        if (!currPage->isInitialized()) {
            currPage->initWindows();
        }

        // select which window to wait for input
        int currWindowID = _pcb->currentWindow();

        // update the page if received input 
        currPage->update(currWindowID); 

        // render the page
        currPage->render();
    }

    // clean windows
    _resources.clean();

    clrtoeol();
    refresh();
    endwin();

}

#include "pcb.h"

PageControlBlock::PageControlBlock(std::size_t defaultPage, int currentWindow) :
    _currentPage(defaultPage),
    _currentWindow(currentWindow), 
    _appStopped(false)
{}

void Page::signalAppStop() noexcept {
    _pcb->stopApp();
}
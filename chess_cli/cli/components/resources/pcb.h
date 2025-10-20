#pragma once

#include "../window/window.h" // TODO: REPLACE LATER AFTER CMAKELISTS.TXT

class PageControlBlock {
public:

    PageControlBlock(std::size_t defaultPage, int currentWindow = 0);     

    inline std::size_t currenntPageIdx() const noexcept {
        return _currentPage;
    } 

    inline int currentWindow() const noexcept {
        return _currentWindow;
    }

    void shiftPageIdx(std::size_t newPageIdx) noexcept {
        _currentPage = newPageIdx;
    }

    void shiftWindow(int newWindow) noexcept {
        _currentWindow = newWindow;
    }

    inline bool appStopped() const {
        return _appStopped;
    }

    void stopApp() {
        _appStopped = true;
    }

private:
    
    std::size_t _currentPage;

    int _currentWindow;

    bool _appStopped;
};

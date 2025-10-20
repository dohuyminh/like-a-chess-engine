#pragma once

#include <curses.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_map>

class Page;
class PageControlBlock;

class Window {
public:

    Window(std::shared_ptr< Page > page, int activateKey, int ty, int tx, int height, int width);

    inline int activateKey() const noexcept {
        return _activateKey;
    }

    inline bool init() noexcept {
        
        // if the window has already been initialized, there is no point
        if (_window) {
            return false;
        }

        // if this is not root window
        if (_activateKey != 27) {
            _window = newwin(_height, _width, _ty, _tx);
            keypad(_window, TRUE);
            nodelay(_window, TRUE);
        }
        else {
            _window = stdscr;
            keypad(_window, TRUE);
        }
        return true;
    }

    inline bool free() noexcept {
        
        // if the window has already been freed
        if (!_window) {
            return false;
        }

        if (_activateKey == 27) {
            // root window - do not delete stdscr
            _window = nullptr;
            return true;
        }
        delwin(_window);
        _window = nullptr;
        return true;
    }

    inline bool isInitialized() const noexcept {
        return _window != nullptr;
    }

    inline std::optional< int > getInput() const {
        // the curses window must be initialzied before reading input
        if (!_window) {
            throw std::runtime_error("Window; window is not initialzied before reading input");
        }
        int ch = wgetch(_window);
        return ch == ERR ? std::nullopt : std::optional< int >(ch);
    }

    virtual void render() const = 0;

    void update();

    virtual void sendUpdateToPage(int input) = 0;

    virtual ~Window();

protected:

    int _activateKey;

    int _tx, _ty, _width, _height;

    WINDOW* _window = nullptr;    

    std::shared_ptr<Page> _page;

};

class Page {
public:

    Page(std::size_t pageIdx, std::shared_ptr<PageControlBlock> pcb);    
    virtual ~Page();

    inline std::size_t pageIdx() const noexcept {
        return _pageIdx;
    }

    inline bool addWindow(std::unique_ptr<Window> win) noexcept {
        if (_windows.count(win->activateKey())) {
            return false;
        }
        _windows[win->activateKey()] = std::move(win);
        return true;
    }  

    inline bool removeWindow(int activateKey) noexcept {
        if (!_windows.count(activateKey)) {
            return false;
        }
        _windows.erase(activateKey);
        return true;    
    }

    void initWindows() noexcept;

    bool isInitialized() const noexcept;

    virtual void render() = 0;    

    void update(int currentWindow);

    virtual void performUpdate() = 0;

    void signalAppStop() noexcept;

    void clean() noexcept;

protected: 

    std::size_t _pageIdx;

    std::unordered_map< int, std::unique_ptr<Window> > _windows;

    std::shared_ptr< PageControlBlock > _pcb;
};


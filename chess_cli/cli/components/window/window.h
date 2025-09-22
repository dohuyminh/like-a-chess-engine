#pragma once

#include <curses.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_map>

class Page;

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

        _window = newwin(_height, _width, _ty, _tx);
        return true;
    }

    inline bool free() noexcept {
        
        // if the window has already been freed
        if (!_window) {
            return false;
        }

        delwin(_window);
        _window = nullptr;
        return true;
    }

    inline std::optional< int > getInput() const {
        // the curses window must be initialzied before reading input
        if (!_window) {
            throw std::runtime_error("Window; window is not initialzied before reading input");
        }
        int ch = wgetch(_window);
        return ch == ERR ? std::optional< int >(ch) : std::nullopt;
    }

    virtual void render() const = 0;

    void update();

    virtual void sendUpdateToPage(int input);

protected:

    int _activateKey;

    int _tx, _ty, _width, _height;

    WINDOW* _window = nullptr;    

    std::shared_ptr< Page > _page;

};

class Page {
public:

    Page() = default;    

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

    virtual void render() = 0;    

    virtual void update() = 0;

protected: 

    std::unordered_map< int, std::unique_ptr<Window> > _windows;

};

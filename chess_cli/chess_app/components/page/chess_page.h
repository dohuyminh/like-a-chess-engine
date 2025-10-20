#pragma once

#include "window.h"
#include "arena.h"

class ChessPage : public Page {
public:

    ChessPage(std::size_t pageIdx, std::shared_ptr<PageControlBlock> pcb, std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    void render() override;

    inline Arena& arena() { return _arena; }

    void performUpdate() override;

private:

    Arena _arena;

};

class ChessRootWindow : public Window {
public:
    ChessRootWindow(std::shared_ptr<Page> page);

    void render() const override;

    void sendUpdateToPage(int input) override;

};

class BoardWindow : public Window {
public:

    BoardWindow(std::shared_ptr<Page> page, Color orientation = Color::WHITE);

    void render() const override;

    void sendUpdateToPage(int input) override;

private:
    
    Color _orientation;

    static void initColorSetting();

};

enum class PlayerType {
    HUMAN,
    AI
};

class MovesWindow : public Window {
public:

    MovesWindow(std::shared_ptr<Page> page, PlayerType playerType);

    void render() const override;

    void sendUpdateToPage(int input) override;

private:

    std::size_t _pointer = 0;

    PlayerType _playerType;

};

class LogsWindow : public Window {
public:

    LogsWindow(std::shared_ptr<Page> page);

    void render() const override;

    void sendUpdateToPage(int input) override;

private:

};
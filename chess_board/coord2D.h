#pragma once

#include <cstdint>
#include <functional>
#include <string>

class Vec2D {
public:
    Vec2D(int8_t mvCol, int8_t mvRow);

    inline int8_t mvRow() const {
        return _mvRow;
    }

    inline int8_t mvCol() const {
        return _mvCol;
    }
    
    inline Vec2D& operator*=(int8_t num) {
        this->_mvCol *= num;
        this->_mvRow *= num;
        return *this;
    }

    inline friend Vec2D operator*(Vec2D direction, int8_t sign) {
        return Vec2D(direction._mvCol * sign, direction._mvRow * sign);
    }

    inline operator std::string() const {
        std::string s = "<";
        s += std::to_string(_mvCol);
        s.push_back(',');
        s += std::to_string(_mvRow);
        s.push_back('>');
        return s;
    }

private:
    int8_t _mvRow;
    int8_t _mvCol;
};

class Coord2D {
public:
    Coord2D(char col, int8_t row);
    
    inline int8_t row() const {
        return _row;
    }

    inline char col() const {
        return _col;
    }

    inline friend bool operator==(Coord2D first, Coord2D second) {
        return first._row == second._row && first._col == second._col;
    }

    inline friend bool operator!=(Coord2D first, Coord2D second) {
        return !(first._row == second._row && first._col == second._col);
    }

    inline friend Coord2D operator+(Coord2D point, Vec2D direction) {
        return Coord2D(point._col + direction.mvCol(), point._row + direction.mvRow());
    }

    inline Coord2D& operator+=(Vec2D mv) {
        this->_row += mv.mvRow();
        this->_col += mv.mvCol();
        return *this;
    }

    inline uint8_t toFlatIdx() {
        return Coord2D::BOARD_SIZE * (_row - 1) + (_col - 'A');
    }

    operator std::string();


private:
    
    static constexpr uint8_t BOARD_SIZE = 8;

    int8_t _row;
    char   _col;
};

enum Direction {
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT,
};

const Vec2D vecMap[8] = {
    Vec2D( 0, 1), // UP
    Vec2D( 0,-1), // DOWN
    Vec2D(-1, 0), // LEFT
    Vec2D( 1, 0), // RIGHT
    Vec2D(-1, 1), // UP LEFT
    Vec2D( 1, 1), // UP RIGHT
    Vec2D(-1,-1), // DOWN LEFT
    Vec2D( 1, 1)  // DOWN RIGHT
};

template <>
struct std::hash<Coord2D> {
    std::size_t operator()(Coord2D coord) const noexcept {
        std::size_t h1 = std::hash<int8_t>()(coord.row()), h2 = std::hash<char>()(coord.col());
        return h1 << (h1 - h2);
    }    
};

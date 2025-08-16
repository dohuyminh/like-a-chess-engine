#include "coord2D.h"

#include <stdexcept>

Coord2D::Coord2D() noexcept : _col('a'), _row(1) {}

Coord2D::Coord2D(const char col, const int8_t row) : _row(row), _col(col) {
    if (!(1 <= row && row <= 8 && 'a' <= col && col <= 'h')) {
        std::string msg = "Coordinate must be initialized in form <a-h><1-8>; argument is ";
        msg.push_back(_col);
        msg.push_back(_row + '0');
        throw std::invalid_argument(msg);

    } 
}

Coord2D::operator std::string() {
    std::string rep("__");
    rep[0] = _col;
    rep[1] = static_cast<char>(_row + '0');
    return rep;
}

Vec2D::Vec2D(const int8_t mvCol, const int8_t mvRow) noexcept : _mvCol(mvCol), _mvRow(mvRow) {}

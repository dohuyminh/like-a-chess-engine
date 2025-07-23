#include "coord2D.h"

#include <stdexcept>

Coord2D::Coord2D(const char col, const int8_t row) : _row(row), _col(col) {
    if (!(1 <= row && row <= 8 && 'A' <= col && col <= 'H')) {
        throw std::invalid_argument("Coordinate must be initialized in form {<A-H>,<1-8>}");
    } 
}

Coord2D::operator std::string() {
    std::string rep("{_,_}");
    rep[1] = _col;
    rep[3] = static_cast<char>(_row + '0');
    return rep;
}

Vec2D::Vec2D(const int8_t mvCol, const int8_t mvRow) : _mvCol(mvCol), _mvRow(mvRow) {}

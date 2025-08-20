#include "r_composite.h"

RComposite::RComposite() noexcept : _components() {}

std::vector<Coord2D> RComposite::pieceCanReachSquare(const ChessBoard& state, Coord2D origin) const {
    std::vector<Coord2D> res;
    
    // for each component, get the piece's array of available next squares
    // then add to collection  
    for (const SquareReachability* c: _components) {
        
        std::vector<Coord2D> available = c->pieceCanReachSquare(state, origin);
        res.insert(res.end(), available.begin(), available.end());
    }

    return res;
}

RComposite::~RComposite() {
    for (SquareReachability* c: _components) {
        delete c;
    }
}
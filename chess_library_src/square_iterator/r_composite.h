#pragma once

#include "reachability.h"

#include <stdexcept>

class RComposite final : public SquareReachability {
public:
    
    RComposite() noexcept;

    inline void addComponent(SquareReachability* component) {
        if (!component) {
            throw std::invalid_argument("Null SquareReachability object passed into component addition; object must be created via the \"new\" keyword");
        }
        _components.push_back(component);
    }

    std::vector<Coord2D> pieceCanReachSquare(
        const ChessBoard& state,
        Coord2D origin
    ) const override;

    ~RComposite();

private:
    std::vector< SquareReachability* > _components;
};

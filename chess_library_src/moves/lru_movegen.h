#pragma once

#include "chess_move.h"
#include <functional>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>


typedef std::pair< ChessBoard, Color > LRUKeyType;

template <>
struct std::hash< LRUKeyType > {
    std::size_t operator()(const LRUKeyType& key) const {
        const ChessBoard& board = key.first;
        const Color color = key.second;

        // Base hash from board
        const std::size_t hBoard = std::hash<ChessBoard>{}(board);

        // Fold color (stored as small integral) into 64-bit space
        const uint64_t colorVal = static_cast<uint64_t>(static_cast<unsigned char>(static_cast<Color::_Color>(color)));

        // Combine using a variant of boost's hash_combine
        uint64_t x = static_cast<uint64_t>(hBoard);
        x ^= colorVal + 0x9e3779b97f4a7c15ULL + (x << 6) + (x >> 2);

        // Finalize with SplitMix64 mixer for good avalanche and low collision
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        x ^= (x >> 31);

        return static_cast<std::size_t>(x);
    }
};

namespace internal {

struct LRUNode {
    const LRUKeyType key;
    std::vector< std::shared_ptr< ChessMove > > moves;
    
    LRUNode* prev;
    LRUNode* next;

    LRUNode(const LRUKeyType& k);
};

class LRULegalMoveGen {
public:
    
    LRULegalMoveGen(std::size_t capacity);

    const std::vector< std::shared_ptr< ChessMove> >& get(const ChessBoard& board, Color color);

    ~LRULegalMoveGen();

private:
    
    std::size_t _capacity;
    std::unordered_map< LRUKeyType, LRUNode* > _cache{};
    
    // sentinel nodes
    LRUNode* _begin;
    LRUNode* _end;

public:
    // mark the object as non-copyable
    LRULegalMoveGen(const LRULegalMoveGen&)            = delete;
    LRULegalMoveGen& operator=(const LRULegalMoveGen&) = delete;
};

}

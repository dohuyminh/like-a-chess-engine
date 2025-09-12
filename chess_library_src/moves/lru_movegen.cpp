#include "lru_movegen.h"
#include "get_all_moves.h"
#include <stdexcept>

namespace internal {

LRUNode::LRUNode(const LRUKeyType& k) : 
    key(k),
    prev(nullptr),
    next(nullptr)
{
    const auto& [ b, c ] = k;
    moves = (c == Color::NONE) ? std::vector< std::shared_ptr < ChessMove > >() : getAllMoves(b, c);
}

LRULegalMoveGen::LRULegalMoveGen(std::size_t capacity) : 
    _capacity(capacity)
{
    // cache with 0 capacity is not allowed
    if (_capacity == 0) {
        throw std::invalid_argument("Initializing a cache with no capacity is not allowed");
    }

    // Create sentinel nodes - they should never be null
    _begin = new LRUNode({ ChessBoard(), Color::NONE });
    _end   = new LRUNode({ ChessBoard(), Color::NONE });

    // Link sentinel nodes together
    _begin->next = _end;
    _end->prev = _begin;
}

const std::vector< std::shared_ptr< ChessMove > >& LRULegalMoveGen::get(const ChessBoard& board, Color color) {
    
    LRUKeyType k{ board, color };

    LRUNode* resNode = nullptr;

    // if the corresponding element already exists, simply extract the node
    if (_cache.count(k)) {
        resNode = _cache.at(k);
        
        // readjust neighboring node 
        resNode->prev->next = resNode->next;
        resNode->next->prev = resNode->prev;
    }
    // if otherwise, create a new node
    else {
        _cache[k] = resNode = new LRUNode(k);
    }

    // move the node to most recent 
    resNode->prev = _begin;
    resNode->next = _begin->next;
    _begin->next->prev = resNode;
    _begin->next = resNode;

    // if the _cache has reached full capacity -> remove least recent
    if (_cache.size() > _capacity) {
        LRUNode* leastRecent = _end->prev;
        leastRecent->prev->next = _end;
        _end->prev = leastRecent->prev;
        
        // erase from cache and delete
        _cache.erase(leastRecent->key);
        delete leastRecent;
    }
    
    // return result
    return resNode->moves;
}

LRULegalMoveGen::~LRULegalMoveGen() {
    for (LRUNode* tmp = _begin; tmp; ) {
        LRUNode* del = tmp;
        tmp = tmp->next;
        delete del;
    }
}

}

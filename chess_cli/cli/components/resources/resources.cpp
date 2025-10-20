#include "resources.h"
#include <iostream>

Resources::Resources(std::initializer_list< std::shared_ptr<Page> > pages) {
    for (const auto& page: pages) {
        // if we see duplicate pages (pages sharing the same index -> alert during runtime)
        if (_pageMap.count(page->pageIdx())) {
            std::cerr << "WARNING: Found pages sharing the same index " << page->pageIdx() << "; the most recent page shall shadow the previous one(s)\n";
        }

        _pageMap.insert({ page->pageIdx(), page });
    }
}

void Resources::clean() noexcept {
    for (auto& [_, page] : _pageMap) {
        page->clean();
    }
    _pageMap.clear(); // Clear the map so shared_ptrs are released
}
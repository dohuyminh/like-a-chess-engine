#pragma once

#include "../window/window.h"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>

class Resources {
public:

    Resources(std::initializer_list< std::shared_ptr<Page> > pages);

    inline std::shared_ptr<Page> getPage(std::size_t pageIdx) const {
        // if page does not exist -> raise error
        if (!_pageMap.count(pageIdx)) {
            throw std::invalid_argument("Resources; page index does not match any pages in resources");
        }

        return _pageMap.at(pageIdx);
    }

    inline bool addPage(std::shared_ptr<Page> page) {
        // page collision (pages sharing the same index) cannot cause new page to override old pages 
        if (_pageMap.count(page->pageIdx())) {
            return false;
        }

        _pageMap.insert({ page->pageIdx(), page });
        return true;
    }

    inline bool removePage(std::size_t pageIdx) {
        // cannot delete a page that doesn't exist
        if (!_pageMap.count(pageIdx)) {
            return false;
        }

        _pageMap.erase(pageIdx);
        return true;
    }

    void clean() noexcept;

private:

    std::unordered_map< std::size_t, std::shared_ptr<Page> > _pageMap;

};

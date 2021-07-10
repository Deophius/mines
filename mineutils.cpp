#include "mineutils.h"

namespace Holy {
    void GameData::recount() noexcept {
        for (int ix = 1; ix <= col; ix++) {
            for (int iy = 1; iy <= row; iy++) {
                // only number blocks have second data
                auto& iblock = blocks[ix][iy];
                if (iblock.status != Block::number)
                    continue;
                iblock.second_init = true;
                iblock.elabel = iblock.label;
                iblock.vacant_nei = 0;
                Point{ ix, iy }.for_each_nei8([&, this](Point np) {
                    auto& nblock = blocks[np.x][np.y];
                    if (nblock.status == Block::mine)
                        iblock.elabel--;
                    if (nblock.status == Block::unknown)
                        iblock.vacant_nei++;
                });
            }
        }
    }

    void GameData::recount(Point p) {
        // Don't call this in recount() because of this if clause
        if (!p.valid())
            throw std::out_of_range("p is not valid!");
        auto& block = blocks[p.x][p.y];
        if (block.status != Block::number)
            return;
        block.second_init = true;
        block.elabel = block.label;
        block.vacant_nei = 0;
        p.for_each_nei8([&, this](Point np) {
            auto& nblock = blocks[np.x][np.y];
            if (nblock.status == Block::mine)
                block.elabel--;
            if (nblock.status == Block::unknown)
                block.vacant_nei++;
        });
    }

    void GameData::mark_semiknown(Point p) {
        if (!p.valid())
            throw std::runtime_error("p is not valid!");
        if ((*this)[p].status != Block::unknown)
            throw std::runtime_error("mark_semiknown: p does not refer to an unprobed block!");
        // Mark point p
        (*this)[p].status = Block::semiknown;
        // mark neighbors, to keep invariant, only take action if second_init is
        // true if second_init is false, this will be taken care of in recount()
        p.for_each_nei8([this](Point np) {
            if ((*this)[np].second_init)
                (*this)[np].vacant_nei--;
        });
    }

    void GameData::mark_mine(Point p) {
        if (!p.valid())
            throw std::runtime_error("p is not valid!");
        if ((*this)[p].status != Block::unknown)
            throw std::runtime_error("mark_mine: p does not refer to an unprobed block!");
        // Mark point p
        (*this)[p].status = Block::mine;
        p.for_each_nei8([this](Point np) {
            if ((*this)[np].second_init) {
                (*this)[np].vacant_nei--;
                (*this)[np].elabel--;
            }
        });
        // Decrease the number of mines left
        mines_left--;
    }

    void GameData::unmark_mine(Point p) {
        if (!p.valid())
            throw std::runtime_error("p is not valid!");
        if ((*this)[p].status != Block::mine)
            throw std::runtime_error("Attempting to unmark a non-mine block");
        (*this)[p].status = Block::unknown;
        p.for_each_nei8([this](Point np) {
            if ((*this)[np].second_init) {
                (*this)[np].vacant_nei++;
                (*this)[np].elabel++;
            }
        });
        mines_left++;
    }

    void GameData::unmark_semiknown(Point p) {
        if (!p.valid())
            throw std::runtime_error("p is not valid!");
        if ((*this)[p].status != Block::semiknown)
            throw std::runtime_error("The block about to be unmarked is not marked");
        (*this)[p].status = Block::unknown;
        p.for_each_nei8([this](Point np) {
            Block& nb = (*this)[np];
            if (nb.second_init)
                nb.vacant_nei++;
        });
    }
} // namespace Holy
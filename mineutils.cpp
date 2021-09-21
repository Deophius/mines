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
            throw std::runtime_error(
                "mark_semiknown: p does not refer to an unprobed block!");
        // Mark point p
        (*this)[p].status = Block::semiknown;
        // mark neighbors, to keep invariant, only take action if second_init is
        // true if second_init is false, this will be taken care of in recount()
        p.for_each_nei8([this](Point np) {
            if ((*this)[np].second_init)
                (*this)[np].vacant_nei--;
        });
    }

    bool GameData::mark_semiknown_check(Point p) {
        if (!p.valid())
            throw std::runtime_error("p is not valid!");
        if ((*this)[p].status != Block::unknown)
            throw std::runtime_error(
                "mark_semiknown_check: p does not refer to an unprobed block!");
        // Grab a list of the blocks to be touched (max size = 8)
        // starts from 1
        Point nps[10] = {};
        // how many entries are there in nps
        int cnt = 0;
        p.for_each_nei8([&](Point np) {
            if ((*this)[np].second_init)
                nps[++cnt] = np;
        });
        (*this)[p].status = Block::semiknown;
        // The current neighbor under manipulation
        int curr = 1;
        for (; curr <= cnt; curr++) {
            Block& nei = (*this)[nps[curr]];
            nei.vacant_nei--;
            if (nei.vacant_nei < nei.elabel)
                break;
        }
        if (curr == cnt + 1)
            // All right, loop exited normally
            return true;
        else {
            // Revert the changes we made
            for (; curr >= 1; curr--)
                (*this)[nps[curr]].vacant_nei++;
            (*this)[p].status = Block::unknown;
            return false;
        }
    }

    void GameData::mark_mine(Point p) {
        if (!p.valid())
            throw std::runtime_error("p is not valid!");
        if ((*this)[p].status != Block::unknown)
            throw std::runtime_error(
                "mark_mine: p does not refer to an unprobed block!");
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

    bool GameData::mark_mine_check(Point p) {
        if (!p.valid())
            throw std::runtime_error("p is not valid!");
        if ((*this)[p].status != Block::unknown)
            throw std::runtime_error(
                "mark_mine_check: p does not refer to an unprobed block!");
        // If there are no more mines left, must be wrong
        if (mines_left <= 0)
            return false;
        // Grab the list of neighbors that have had second init
        Point nps[10] = {};
        int cnt = 0;
        p.for_each_nei8([&](Point np) {
            if ((*this)[np].second_init)
                nps[++cnt] = np;
        });
        // make the mark
        (*this)[p].status = Block::mine;
        // Start modifying second_init data
        int curr = 1;
        for (; curr <= cnt; curr++) {
            Block& nei = (*this)[nps[curr]];
            nei.elabel--;
            nei.vacant_nei--;
            // elabel <= vacant_nei because they both decreased
            if (nei.elabel < 0)
                break;
        }
        if (curr == cnt + 1) {
            // No problem occurred
            mines_left--;
            return true;
        } else {
            // Revert
            for (; curr >= 1; curr--) {
                Block& nei = (*this)[nps[curr]];
                nei.elabel++;
                nei.vacant_nei++;
            }
            (*this)[p].status = Block::unknown;
            return false;
        }
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
            throw std::runtime_error(
                "The block about to be unmarked is not marked");
        (*this)[p].status = Block::unknown;
        p.for_each_nei8([this](Point np) {
            Block& nb = (*this)[np];
            if (nb.second_init)
                nb.vacant_nei++;
        });
    }
} // namespace Holy
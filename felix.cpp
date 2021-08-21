#include "solvers.h"
#include <array>
#include <cassert>

namespace {
    // Gets share_cnt map, expressed in an array of ints
    std::array<int, Holy::hash_max>
        get_share_cnt(Holy::GameData& game, Holy::Point p) {
        using namespace Holy;
        std::array<int, Holy::hash_max> share_cnt{ 0 };
        p.for_each_nei8([&](Point vacant) {
            if (game[vacant].status != Block::unknown)
                return;
            vacant.for_each_nei8([&](Point nei2) {
                if (nei2 == p)
                    return;
                if (game[nei2].status != Block::number)
                    return;
                share_cnt[nei2.hash()]++;
            });
        });
        return share_cnt;
    }

    // This helper clicks the appropriate blocks after a deduction
    // has been made.
    void click_blocks(Holy::GameData& game, Holy::Point p, Holy::Point nei2) {
        using namespace Holy;
        // Those kept back are all mines, unused neighbors of center num
        // 1 if kept back, 2 if unused nei of center, 3 if both
        // std::map<Point, unsigned char> roles;
        std::array<unsigned char, hash_max> roles{ 0 };
        p.for_each_nei8([&](Point nei) {
            if (game[nei].status == Block::unknown)
                roles[nei.hash()] = 2; // = because initial value is 0
        });
        nei2.for_each_nei8([&](Point nei) {
            if (game[nei].status == Block::unknown)
                roles[nei.hash()] += 1;
        });
        /* for (const auto& [nei, role] : roles) {
            if (role == 1)
                game.mark_mine(nei);
            if (role == 2) {
                game.mark_semiknown(nei);
            }
        } */
        // The variable used in the loop
        Point nei;
        for (nei.x = 1; nei.x <= col; nei.x++) {
            for (nei.y = 1; nei.y <= row; nei.y++) {
                const int role = roles[nei.hash()];
                if (role == 0)
                    continue;
                if (role == 1)
                    game.mark_mine(nei);
                else if (role == 2)
                    game.mark_semiknown(nei);
            }
        }
    }

    // This function does the work.
    // Returns true if a modification is made
    bool worker(Holy::GameData& game, Holy::Point p) {
        using namespace Holy;
        // p is the center with effective_label 1
        assert(game[p].elabel == 1);
        assert(game[p].status == Block::number);
        auto share_cnt = get_share_cnt(game, p);
        Point nei2;
        /* for (auto [nei2, shared] : share_cnt) {
            // According to strategy, kept_back +1 == elabel is deducible
            const int kept = game[nei2].vacant_nei - shared;
            assert(kept >= 0);
            if (kept + 1 != game[nei2].elabel)
                continue;
            // There's no real use doing extra work
            if (kept == 0 && shared == game[p].vacant_nei)
                continue;
            click_blocks(game, p, nei2);
            // Each center only serves one second-neighbor.
            return true;
        } */
        // Iterate through share_cnt, and find the blocks with data > 0
        for (nei2.x = 1; nei2.x <= col; nei2.x++) {
            for (nei2.y = 1; nei2.y <= row; nei2.y++) {
                int shared = share_cnt[nei2.hash()];
                if (shared == 0)
                    continue;
                // According to strategy, kept_back +1 == elabel is deducible
                const int kept = game[nei2].vacant_nei - shared;
                assert(kept >= 0);
                if (kept + 1 != game[nei2].elabel)
                    continue;
                // There's no real use doing extra work
                if (kept == 0 && shared == game[p].vacant_nei)
                    continue;
                click_blocks(game, p, nei2);
                // Each center only serves one second-neighbor.
                return true;
            }
        }
        return false;
    }
} // namespace

namespace Holy {
    bool felix(GameData& game) {
        bool ret = false;
        for (int ix = 1; ix <= col; ix++) {
            for (int iy = 1; iy <= row; iy++) {
                Point p = { ix, iy };
                if (game[p].status == Block::number && game[p].elabel == 1)
                    ret = worker(game, p) || ret;
            }
        }
        return ret;
    }
} // namespace Holy
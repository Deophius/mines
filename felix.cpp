#include "solvers.h"
#include <cassert>
#include <map>

namespace {
    // Gets share_cnt map
    std::map<Holy::Point, int>
        get_share_cnt(Holy::GameData& game, Holy::Point p) {
        using namespace Holy;
        // FIXME: Can we make map more effective by static thread_local?
        // FIXME: Or can we switch to a bitset of 480 bits?
        std::map<Point, int> share_cnt;
        p.for_each_nei8([&](Point vacant) {
            if (game[vacant].status != Block::unknown)
                return;
            vacant.for_each_nei8([&](Point nei2) {
                if (nei2 == p)
                    return;
                if (game[nei2].status != Block::number)
                    return;
                if (share_cnt.find(nei2) != share_cnt.end())
                    share_cnt[nei2]++;
                else
                    share_cnt[nei2] = 1;
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
        std::map<Point, unsigned char> roles;
        p.for_each_nei8([&](Point nei) {
            if (game[nei].status == Block::unknown)
                roles[nei] = 2; // = because initial value is 0
        });
        nei2.for_each_nei8([&](Point nei) {
            if (game[nei].status == Block::unknown)
                roles[nei] += 1;
        });
        for (const auto& [nei, role] : roles) {
            if (role == 1)
                game.mark_mine(nei);
            if (role == 2) {
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
        for (auto [nei2, shared] : share_cnt) {
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
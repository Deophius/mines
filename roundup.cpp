#include "solvers.h"
#include <cassert>

namespace Holy {
    namespace {
        bool do_work(GameData& game, Butterfly& butt, Point p) {
            if (game[p].status != Block::number)
                return false;
            if (game[p].vacant_nei == 0)
                return false;
            if (game[p].vacant_nei == game[p].elabel) {
                p.for_each_nei8([&](Point np) {
                    // mark a mine if this is vacant
                    if (game[np].status == Block::unknown) {
                        game.mark_mine(np);
                        // Butterfly doesn't support right click
                    }
                });
                return true;
            }
            if (game[p].elabel == 0) {
                p.for_each_nei8([&](Point np) {
                    // mark a number if it is vacant,
                    // and don't forget to call recount!
                    if (game[np].status == Block::unknown) {
                        game.mark_semiknown(np);
                        auto read = butt.click(np);
                        // deterministic
                        assert(read);
                        game[np].label = *read;
                        game[np].status = Block::number;
                        game.recount(np);
                    }
                });
                return true;
            }
            return false;
        }
    }

    bool roundup(GameData& game, Butterfly& butt) {
        bool ret = false;
        for (int ix = 1; ix <= col; ix++) {
            for (int iy = 1; iy <= row; iy++) {
                // Be careful of short circuit
                ret = do_work(game, butt, { ix, iy }) || ret;
            }
        }
        return ret;
    }
}
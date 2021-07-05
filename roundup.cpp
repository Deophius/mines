#include "solvers.h"
#include <cassert>

namespace Holy {
    namespace {
        bool do_work(GameData& game, Point p) {
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
                    // recounting done in accio
                    if (game[np].status == Block::unknown)
                        game.mark_semiknown(np);
                });
                return true;
            }
            return false;
        }
    } // namespace

    bool roundup(GameData& game) {
        bool ret = false;
        for (int ix = 1; ix <= col; ix++) {
            for (int iy = 1; iy <= row; iy++) {
                // Be careful of short circuit
                ret = do_work(game, { ix, iy }) || ret;
            }
        }
        return ret;
    }
} // namespace Holy
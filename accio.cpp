#include "solvers.h"
#include <cassert>
// #include <iostream>
#include <queue>
#include <set>

namespace {
    using Uninit = std::set<Holy::Point>;
    using namespace Holy;

    // Does the clicking work at point p
    // assumes that the block at p is semiknown, so it has its neighbors notified
    // Doesn't modify uninit
    bool do_click(GameData& game, Butterfly& butt, bool det, Point p) {
        assert(game[p].status == Block::semiknown);
        auto read = butt.click(p);
        if (det && !read)
            std::terminate();
        if (!det && !read)
            return false;
        // So now read contains a value
        // Already updated neighbors' info, just pull in value
        auto& block = game[p];
        block.label = *read;
        block.status = Block::number;
        return true;
    }

    // When a new continent is discovered, do bfs
    bool bfs(GameData& game, Butterfly& butt, bool det, Point p, Uninit& uninit) {
        assert(game[p].status == Block::number);
        assert(game[p].label == 0);
        std::queue<Point> q;
        // vis is set when elements are pushed into q
        std::bitset<col* row + 5> vis = 0;
        q.push(p);
        vis[p.hash()] = true;
        do {
            // Current point under investigation
            Point p = q.front();
            q.pop();
            // std::cerr << "Currently: " << p.x << ' ' << p.y << '\n';
            // Mark it as semiknown if it is unknown
            // if p is empty or semiknown, then p is not the initial point
            if (game[p].status == Block::unknown) {
                game.mark_semiknown(p);
            }
            // Read the data from p if it is not the initial point
            if (game[p].status != Block::number) {
                if (!do_click(game, butt, det, p))
                    // non det, error move
                    return false;
            }
            if (game[p].label == 0) {
                // Continue expanding
                p.for_each_nei8([&](Point np) {
                    if (!vis[np.hash()]
                        && (game[np].status == Block::unknown
                            || game[np].status == Block::semiknown)) {
                        q.push(np);
                        vis[np.hash()] = true;
                    }
                });
            } else {
                // Another block to recount
                uninit.insert(p);
            }
        } while (!q.empty());
        return true;
    }
} // namespace

namespace Holy {
    bool accio(GameData& game, Butterfly& butt, bool det) {
        Uninit uninit;
        for (int ix = 1; ix <= col; ix++) {
            for (int iy = 1; iy <= row; iy++) {
                Point p = { ix, iy };
                // only interested in those that are semiknown
                if (game[p].status != Block::semiknown)
                    continue;
                uninit.insert(p);
                if (do_click(game, butt, det, p) == false)
                    return false;
                // After this game[p].status == number
                if (game[p].label)
                    continue;
                // New continent discovered
                bfs(game, butt, det, p, uninit);
            }
        }
        // Now call recount for each in uninit
        for (auto p : uninit) {
            game.recount(p);
        }
        return true;
    }
} // namespace Holy
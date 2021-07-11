#include "solvers.h"
#include <bitset>
#include <cassert>

namespace {
    using namespace Holy;

    // Finds the frontier where the search takes place
    // Output written to front
    // No need to communicate with butterfly here
    void find_front(const GameData& game, Frontier& front) {
        Checklist vis;
        for (int ix = 1; ix <= col; ix++) {
            for (int iy = 1; iy <= row; iy++) {
                Point p{ ix, iy };
                if (game[p].status == Block::number && game[p].elabel) {
                    // Which means there is another mine hidden in nei of p
                    p.for_each_nei8([&](Point np) {
                        if (game[np].status == Block::unknown)
                            vis[np.hash()] = true;
                    });
                }
            }
        }
        front.clear();
        for (int ix = 1; ix <= col; ix++) {
            for (int iy = 1; iy <= row; iy++) {
                Point p{ ix, iy };
                if (vis[p.hash()])
                    front.push_back(p);
            }
        }
    }

    // Generally we don't cut up the frontier because of mines_left

    // Check for inconsistency with center p, also if mines_left < 0
    bool local_ok(const GameData& game, Point p) {
        if (game.mines_left < 0)
            return false;
        for (int ix = p.x - 3; ix <= p.x + 3; ix++) {
            for (int iy = p.y - 3; iy <= p.y + 3; iy++) {
                Point p{ ix, iy };
                if (!p.valid())
                    continue;
                if (game[p].status == Block::unknown && game[p].label) {
                    if (game[p].vacant_nei < game[p].elabel)
                        return false;
                }
            }
        }
        return true;
    }

    // The actual searching happens here.
    // We cannot call felix and roundup here because we'll have trouble
    // redoing if we guessed wrong
    // We're at the kth element of frontier.
    // If we find a reasonable solution, append it to ans.
    // FIXME: Pay attention to move semantics after first dev
    void
        dfs(const GameData& game,
            const Frontier& front,
            int k,
            std::vector<GameData>& ans) {
        if (k == front.size()) {
            // Reached end of recursion, success
            ans.push_back(game);
            if (ans.size() >= 10000)
                throw std::runtime_error("Too much recursion!");
            return;
        }
        Point p = front[k];
        // First check if this is set in previous guesses
        if (game[p].status != Block::unknown) {
            // move on
            dfs(game, front, k + 1, ans);
            return;
        }
        // First guess: mine
        {
            // copy it down because I'm having trouble restoring it
            GameData game2 = game;
            game2.mark_mine(p);
            do {
                while (roundup(game2))
                    ;
            } while (felix(game2));
            if (local_ok(game2, p))
                dfs(game2, front, k + 1, ans);
        }
        // Second guess: not a mine
        {
            GameData game2 = game;
            game2.mark_semiknown(p);
            do {
                while (roundup(game2))
                    ;
            } while (felix(game2));
            if (local_ok(game2, p))
                dfs(game2, front, k + 1, ans);
        }
    }
} // namespace

namespace Holy {
    std::optional<MineChance> john(GameData& game) {
        std::vector<GameData> ans;
        Frontier front;
        find_front(game, front);
        dfs(game, front, 0, ans);
        assert(ans.size());
        // The result of this call
        MineChance mc;
        mc.fill(0); // to be safe
        for (const auto& solution : ans) {
            for (const auto& p : front) {
                if (solution[p].status == Block::mine)
                    mc[p.hash()]++;
            }
        }
        bool det = false;
        // Check for deterministic behaviours
        for (const auto& p : front) {
            const int chance = mc[p.hash()];
            if (chance == ans.size()) {
                game.mark_mine(p);
                det = true;
            } else if (chance == 0) {
                game.mark_semiknown(p);
                det = true;
            }
        }
        if (det)
            return std::nullopt;
        return mc;
    }
} // namespace Holy
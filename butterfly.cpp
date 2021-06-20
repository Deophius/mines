#include "butterfly.h"
#include <algorithm>
#include <chrono>
#include <queue>
#include <random>
#include <vector>

namespace Holy {
    void Butterfly::start_game(Point p) {
        mInGame = true;
        for (auto& row : mMined)
            row = 0;
        for (auto& row : mExpose)
            row = 0;
        for (auto& row : mLabel)
            row.fill(0);
        std::vector<Point> possible;
        possible.reserve(col * row - 4);
        const auto adjacent = [p](Point np) {
            return -1 <= p.x - np.x and p.x - np.x <= 1 and -1 <= p.y - np.y
                and p.y - np.y <= 1;
        };
        for (int ix = 1; ix <= col; ix++) {
            for (int iy = 1; iy <= row; iy++) {
                if (not adjacent({ ix, iy }))
                    possible.push_back({ ix, iy });
            }
        }
        using std::chrono::system_clock;
        std::mt19937 gen(system_clock::now().time_since_epoch().count());
        std::shuffle(std::begin(possible), std::end(possible), gen);
        for (int i = 1; i <= mines; i++) {
            Point& p = possible[i];
            mMined[p.x][p.y] = true;
        }
        // Now accumulate the labels
        for (int i = 1; i <= mines; i++) {
            Point& p = possible[i];
            p.for_each_nei8([this](Point np) {
                if (not mMined[np.x][np.y])
                    mLabel[np.x][np.y]++;
            });
        }
        // Do the first click
        click(p);
    }

    std::optional<int> Butterfly::read(Point p) const {
        if (not mInGame)
            throw std::logic_error("Has not started game!");
        if (not mExpose[p.x][p.y])
            return std::nullopt;
        return std::make_optional(mLabel[p.x][p.y]);
    }

    std::optional<int> Butterfly::click(Point p) {
        if (!mInGame)
            throw std::logic_error("Has not started game!");
        if (mMined[p.x][p.y]) {
            mInGame = false;
            return std::nullopt;
        }
        // If this is not mined, then push it into a queue for processing
        // This is the BFS manner of doing things.
        std::queue<Point> q;
        // vis to eliminate double-checks
        std::bitset<col* row + 2> vis = 0;
        q.push(p);
        vis[p.hash()] = true;
        while (!q.empty()) {
            // Every point in q is not a mine, but possibly 0
            // and vis[p] is true
            Point p = q.front();
            q.pop();
            // Now p is shielded by local variable
            mExpose[p.x][p.y] = true;
            if (mLabel[p.x][p.y] == 0)
                p.for_each_nei8([&, this](Point np) {
                    if (!mMined[np.x][np.y] && !vis[np.hash()]) {
                        q.push(np);
                        vis[np.hash()] = true;
                    }
                });
        }
        return std::make_optional(mLabel[p.x][p.y]);
    }

    bool Butterfly::verify() const {
        for (int ix = 1; ix <= col; ix++) {
            for (int iy = 1; iy <= row; iy++) {
                if (!mMined[ix][iy] && !mExpose[ix][iy])
                    return false;
            }
        }
        return true;
    }

    bool Butterfly::in_game() const noexcept {
        return mInGame;
    }
} // namespace Holy
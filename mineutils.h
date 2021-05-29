#ifndef MINEUTILS_H
#define MINEUTILS_H

#include <array>
#include <stdexcept>

namespace Holy {
    // parameters of minesweeper game
    constexpr int col = 30, row = 16, mines = 99;

    // structure of a point (simple aggregate)
    struct Point {
        int x, y;

        // If *this is inside the game board, returns true
        inline bool valid() const noexcept {
            return 1 <= x && x <= col && 1 <= y && y <= row;
        }

        // Returns the hash of *this
        inline int hash() const noexcept {
            return x + y * col;
        }

        // Carry out an operation for the valid of 8 neighbors of p
        // fn(Point) should be a valid invocation
        template <typename Fn>
        void for_each_nei8(Fn&& fn) {
            // template functions must be defined in header
            static_assert(std::is_invocable_v<Fn, Point>, "Should meet type req!");
            constexpr int dx[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
            constexpr int dy[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
            for (int i = 0; i < 8; i++) {
                Point np{ x + dx[i], y + dy[i] };
                if (np.valid())
                    fn(np);
            }
        }

        // Carry out an operation for the valid of 4 neighbors of p
        // fn should meet same type requirements
        template <typename Fn>
        void for_each_nei4(Fn&& fn) {
            static_assert(std::is_invocable_v<Fn, Point>, "Should meet type req!");
            constexpr int dx[] = { 0, 0, 1, -1 };
            constexpr int dy[] = { 1, -1, 0, 0 };
            for (int i = 0; i < 4; i++) {
                Point np{ x + dx[i], y + dy[i] };
                if (np.valid())
                    fn(np);
            }
        }
    };

    // Data structure of a block
    // This struct stores the basic data of a block
    struct Block {
        // status of block
        // semiknown is a state where you know a block contains a number,
        // but do not know what the number is exactly.
        enum Status {
            unknown, mine, number, semiknown
        };
        Status status = unknown;
        // label read from mock-server (0 if status != number)
        int label = 0;
        // tags whether second hand data is initialized
        // FIXME: Perhaps put second_init into private and befriend GameData::recount() -- difficult
        // Or put this info into GameData
        bool second_init = false;
        // effective label (0 if status != number)
        int elabel = 0;
        // the number of vacant neighbors
        int vacant_nei = 0;
    };

    // This class stores the basic data of the game
    struct GameData {
        // Array of the blocks
        std::array<std::array<Block, row + 1>, col + 1> blocks;

        // A shorthand for accessing a given Block
        // Does not check for out_of_bound errors, to make noexcept promise
        // According to language standard, only one argument
        inline Block& operator[] (Point p) noexcept {
            return blocks[p.x][p.y];
        }

        inline const Block& operator[] (Point p) const noexcept {
            return blocks[p.x][p.y];
        }

        // (Re)initializes satellite data for number blocks
        void recount() noexcept;

        // Initializes satellite data for a specific block
        // throws out of range if p is not valid
        void recount(Point p);

        // Marks a block as semiknown (number, but unknown label)
        // It decreases the neighbors' vacant_nei, but not elabel
        // Assumes that p is an unprobed block and not out of bound,
        // if not so, throws std::runtime_error
        void mark_semiknown(Point p);

        // Marks a block as mine
        // It decreases the neighbors' vacant_nei and elabel
        // Assumes that p is an unprobed block and not out of bound,
        // otherwise raises std::runtime_error
        void mark_mine(Point p);
    };
}

#endif
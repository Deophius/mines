#ifndef BUTTERFLY_H
#define BUTTERFLY_H

#include "mineutils.h"
#include <bitset>
#include <optional>
#include <random>

namespace Holy {
    // This class serves as a mock-minesweeper program
    class Butterfly {
    public:
        // Initializes Butterfly, especially the gen engine
        Butterfly();

        // Copy operations are not permitted.
        Butterfly& operator=(const Butterfly& src) = delete;

        // Copy operations are not permitted.
        Butterfly(const Butterfly& src) = delete;

        // Move operations, default
        // @exception None
        Butterfly& operator=(Butterfly&& src) noexcept = default;

        // Move operations, default
        // @exception None
        Butterfly(Butterfly&& src) noexcept = default;

        // Default virtual destructor
        virtual ~Butterfly() noexcept = default;

        // starts a game with click at p(x, y)
        // Plot mines so that (x-1, y-1) to (x+1, y+1) are cleared
        // Will spend some time plotting the game
        void start_game(Point p);

        // Reads from a block, to simulate real minesweeper games, tells the
        // difference from empty and 0 by returning as optional
        // If unknown, optional is empty;
        // If contains a mine, same as unknown because you cannot "right click"
        // If probed, returns the number
        // If start_game has not been called for this game, throws
        // std::logic_error
        std::optional<int> read(Point p) const;

        // "Left clicks" at the given point, note there is right click
        // counterpart.
        // If start_game has not been called for this game, throws
        // std::logic_error.
        // If this click probes a mine, returns nullopt
        // If probes a number, returns the label, which may be 0
        std::optional<int> click(Point p);

        // Verifies that you have won the game
        // Returns true only if you have probed all blocks with a label of
        // [0,8], which consequently leaves blocks with mine empty
        // Otherwise, returns false Throws std::logic_error if start_game has not been
        // called
        bool verify() const;

        // Tells you whether currently in a game.
        // Returns the value as dictated in mInGame
        bool in_game() const noexcept;

    private:
        // Invariant: verify() that returns true sets this to false
        // clicking a mine causes end of game
        // only start_game() sets this to true
        // click, verify, read should function only if this is true
        bool mInGame = false;

        // invariant: inits in start_game, and not changed throughout the game
        // 1 if there is a mine
        std::array<std::bitset<row + 1>, col + 1> mMined;

        // invariant: inits in start_game to all zero,
        // extends in click(), read by read() to control access
        // 1 if this block is OK to expose
        std::array<std::bitset<row + 1>, col + 1> mExpose;

        // invariant: inits in start_game in accordance with mMined
        // unchanged in a game
        std::array<std::array<int, row + 1>, col + 1> mLabel;

        // The random generator to be used
        std::mt19937 mGen;
    };
} // namespace Holy

#endif // BUTTERFLY_H
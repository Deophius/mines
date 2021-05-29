#ifndef SOLVERS_H
#define SOLVERS_H

#include "butterfly.h"

// Because now reading and solving costs nothing, we can directly
// transfer data to and from the butterfly immediately

namespace Holy {
    /// @brief Deterministic solver

    /// For all blocks with neighbors either all empty or all mined,
    /// probe them and returns.
    /// This can be called several times to squeeze the easiest case
    /// out of the board.
    /// @param game -- the game_data structure
    /// @param butt -- the corresponding butterfly
    /// @returns true if this call made a difference,
    /// @returns false otherwise
    /// @exception This function does not throw exceptions on its own behalf,
    /// but lower-level exceptions can be transmitted up.
    bool roundup(GameData& game, Butterfly& butt);
}

#endif
#ifndef SOLVERS_H
#define SOLVERS_H

#include "butterfly.h"

// Because now reading and solving costs nothing, we can directly
// transfer data to and from the butterfly immediately
/// @file solvers.h Solvers' declarations
/// @attention All solvers expect that the GameData structure is up to date
/// and will ensure it is still up to date when the solver exits.

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
    /// @warning Terminates when a design error happens
    bool roundup(GameData& game, Butterfly& butt);

    /// @brief Deterministic solver
    ///
    /// For all blocks with effective label 1, probe them using a mechanism
    /// commonly used in human gameplay: deduction by using a subset of the
    /// vacant blocks of the center
    /// @param game -- the game data structure
    /// @param butt -- the butterfly used
    /// @returns true if this call made a difference,
    /// @returns false otherwise.
    /// @exception This function only transmits exceptions.
    /// @warning Terminates when an unexpected bad move is taken.
    bool felix(GameData& game, Butterfly& butt);

    /// @brief Helper to transfer data from butterfly
    ///
    /// After the solver has made up its about mind which blocks to probe, it
    /// will call mark_semiknown() and mark_mine(), which maintains the satellite
    /// data for blocks already probed. This function's job is to get data from
    /// butterfly, making sure the newly appeared blocks' recount() called, and
    /// new continents are handled correctly.
    /// @param game -- the game data with semiknown blocks
    /// @param butt -- the butterfly
    /// @param det -- whether the caller is determinisic
    /// @returns true if not lost, false if lost (det == false)
    /// @returns true (det == true)
    /// @exception This function only transmits exceptions.
    /// @warning Contains asserts that cause program to crash.
    bool accio(GameData& game, Butterfly& butt, bool det);
} // namespace Holy

#endif
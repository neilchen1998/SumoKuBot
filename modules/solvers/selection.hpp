#ifndef MODULES_SOLVERS_SUDOKU_SELECTION_HPP
#define MODULES_SOLVERS_SUDOKU_SELECTION_HPP

#include <cstddef> // size_t
#include <cstdint> // uint16_t
#include <limits>  // std::numeric_limits<size_t>::max

/// @brief The selection
struct Selection
{
    size_t r = std::numeric_limits<size_t>::max();
    size_t c = std::numeric_limits<size_t>::max();

    /// @brief The candidates in the mask form
    uint16_t mask = 0U;

    /// @brief TRUE if there is no other options
    bool deadEnd = false;
};

#endif // MODULES_SOLVERS_SUDOKU_SELECTION_HPP

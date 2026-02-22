#ifndef INCLUDE_BOARD_BOARDLIB_H_
#define INCLUDE_BOARD_BOARDLIB_H_

#include <vector>   // std::vector

#include <fmt/core.h>   // fmt::print

/// @brief Print a Sudoku board
/// @tparam T The element type of the board (must support << operator)
/// @param board The given Sudoku board
template <typename T>
inline void PrintBoard(const std::vector<std::vector<T>>& board)
{
    for (auto& row : board)
    {
        for (auto& ele : row)
        {
            fmt::print("{} ", ele);
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

#endif // INCLUDE_BOARD_BOARDLIB_H_

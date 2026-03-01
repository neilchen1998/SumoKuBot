#ifndef INCLUDE_BOARD_BOARDLIB_H_
#define INCLUDE_BOARD_BOARDLIB_H_

#include <vector>   // std::vector

#include <boost/functional/hash.hpp>    // boost::hash_combine
#include <fmt/core.h>   // fmt::print
#include <fmt/ranges.h> // fmt::print for std::array

template<typename T>
concept BoardType = std::integral<T> && !std::same_as<T, bool>;

struct Point
{
    size_t x;
    size_t y;

    bool operator==(const Point& other) const
    {
        return ((x == other.x) && (y == other.y));
    }
};

struct PointHasher
{
    std::size_t operator()(const Point& p) const noexcept
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, p.x);
        boost::hash_combine(seed, p.y);
        return seed;
    }
};

/// @brief Print a Sudoku board
/// @tparam T The element type of the board (must support << operator)
/// @param board The given Sudoku board
template <BoardType T>
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

/// @brief Counts the number of combinations that sum up to the target
/// @param target The sum target
/// @param k The amount of numbers that can be used
/// @param num The current number
/// @return int the number of combinations
consteval int CountCombinations(int target, int k, int num = 1)
{
    // If we have used all the amount of numbers
    if (k == 0)
    {
        return (target == 0) ? 1 : 0;
    }

    // If the target reaches to negative that means we have overshot
    if (target < 0)
    {
        return 0;
    }

    // If the current number is greater than 9, that means we have overshot
    if (num > 9)
    {
        return 0;
    }

    int count = 0;

    for (int i = num; i <= 9; ++i)
    {
        count += CountCombinations(target - i, k - 1, i + 1);
    }

    return count;
}

#endif // INCLUDE_BOARD_BOARDLIB_H_

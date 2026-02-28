#ifndef INCLUDE_BOARD_BOARDLIB_H_
#define INCLUDE_BOARD_BOARDLIB_H_

#include <vector>   // std::vector

#include <boost/functional/hash.hpp>    // boost::hash_combine
#include <fmt/core.h>   // fmt::print

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

#endif // INCLUDE_BOARD_BOARDLIB_H_

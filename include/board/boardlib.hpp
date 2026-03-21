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

    // Use the default == operator
    bool operator==(const Point&) const = default;
};

struct BoostPointHasher
{
    std::size_t operator()(const Point& p) const noexcept
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, p.x);
        boost::hash_combine(seed, p.y);
        return seed;
    }
};

/// @brief Defines a type that is hashable
template<typename T>
concept Hashable = requires (T a)
{
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

/// @brief Hashes a value with a seed
/// @tparam T Hashable
/// @param seed The seed
/// @param v The value
template <Hashable T>
void hash_combine(std::size_t& seed, const T& v)
{
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct PointHasher
{
    std::size_t operator()(const Point& p) const noexcept
    {
        std::size_t seed = 0;
        hash_combine(seed, p.x);
        hash_combine(seed, p.y);
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
consteval int CalCombinations(int target, int k, int num = 1)
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
        count += CalCombinations(target - i, k - 1, i + 1);
    }

    return count;
}

struct CombinationsLUT
{
    // data[i][j]: the number of unique combinations of i distinct digit(s) that sum up to exactly j
    int data[10][46] {};

    constexpr CombinationsLUT()
    {
        // The base case
        // There is only one way to sum up to zero
        data[0][0] = 1;

        // Iterate from number 1 to 9
        for (int digit = 1; digit <= 9; ++digit)
        {
            // Iterate the number of digits and target in reverse to ensure each digit is used only once
            // NOTE: Iterating forward from k = 1 to k = 9 would allow the current digit to be added
            // to a sum that already includes it, leading duplicated digits.
            // Reversing the loops ensures we only build results from the previous iteration
            for (int k = 9; k >= 1; --k)
            {
                for (int t = 45; t >= digit; --t)
                {
                    data[k][t] += data[k - 1][t - digit];
                }
            }
        }
    }

    /// @brief Finds the number of unique combinations of k distinct digits that sum up to a given target from the data
    /// @param target The target sum
    /// @param k The number of distinct digit(s)
    /// @return The total number of unique combinations
    constexpr int get(int target, int k) const
    {
        if (target < 0 || target > 45 || k < 0 || k > 9) return 0;
        return data[k][target];
    }
};

/// @brief Finds the number of unique combinations of k distinct digits that sum up to a given target
/// @param target The target sum
/// @param k The number of distinct digit(s)
/// @return The total number of unique combinations
constexpr int CountCombinations(int target, int k)
{
    static constexpr CombinationsLUT table {};
    return table.get(target, k);
}

struct PossibleNumbersTable
{
    static constexpr int MAX_SUM   = 45;
    static constexpr int MAX_COUNT = 9;

    std::array<std::array<uint16_t, MAX_SUM + 1>, MAX_COUNT + 1> table {};

    consteval PossibleNumbersTable()
    {
        // Loop through each digit from 1 to 9
        // NOTE: the lowest bit of i represents the number 1, the second lowest bit represents the number 2, etc.
        // 0b1: 1 -> there is no trailing zero
        // 0b10: 2 -> there is one trailing zero
        for (uint16_t i = 0U; i < (1U << MAX_COUNT); ++i)
        {
            // Get the count of the numbers
            int cnt = std::popcount(i);

            uint16_t mask = i;
            uint16_t sum = 0;

            uint16_t tmp = i;
            while (tmp)
            {
                sum += (std::countr_zero(tmp) + 1); // converts to number
                tmp &= (tmp - 1);   // removes the rightmost 1 bit
            }

            // Store the current count and the sum into the table
            table[cnt][sum] |= (mask << 1);
        }
    }

    /// @brief Finds all the candidates of k distinct digits that sum up to a given target
    /// @param target The target sum
    /// @param k The number of distinct digit(s)
    /// @return All the candidates in mask format
    constexpr uint16_t get(size_t target, size_t count) const
    {
        if (count > MAX_COUNT || target > MAX_SUM)  return 0;

        return table[count][target];
    }
};

/// @brief Finds all the candidates of k distinct digits that sum up to a given target
/// @param target The target sum
/// @param k The number of distinct digit(s)
/// @return All the candidates in mask format
constexpr uint16_t GetPossibleNumbersMask(size_t target, size_t count)
{
    static constexpr PossibleNumbersTable table{};
    return table.get(target, count);
}

#endif // INCLUDE_BOARD_BOARDLIB_H_

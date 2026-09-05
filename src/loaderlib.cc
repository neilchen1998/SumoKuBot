#include "loader/loaderlib.hpp"

// #include <algorithm>
#include <expected> // std::expected
#include <filesystem>   // std::filesystem
#include <numeric>  // std::accumulate
#include <ranges>   // std::ranges::any_of
#include <unordered_set>    // std::unordered_set
#include <vector>    // std::vector

#include <nlohmann/json.hpp>    // nlohmann::json

#include "math/mathlib.hpp" // PointHasher

namespace fs = std::filesystem;

/// @brief Gets the directory of the test data
/// @return Test data directory
std::string GetTestDataPath()
{
    return TEST_DATA_DIR;
}

std::expected<void, std::string> ValidateSudokuPuzzle(const SudokuPuzzleData& puzzle)
{
    const size_t N = puzzle.N;

    if (N == 0)
    {
        return std::unexpected(fmt::format("The board size must be greater than zero."));
    }

    if (puzzle.board.empty())
    {
        return std::unexpected(fmt::format("The board is empty."));
    }

    if (puzzle.board.size() != N)
    {
        return std::unexpected(fmt::format("The board contains {} row(s), but N is {}", puzzle.board.size(), N));
    }

    for (const auto& row : puzzle.board)
    {
        if (N != row.size())
        {
            return std::unexpected(fmt::format("The board is not square ({} != {}).", N, row.size()));
        }

        // Check if any of the element inside the board is out of bound
        if (std::ranges::any_of(row, [N](int val) { return val < 0 || val > static_cast<int>(N); } ))
        {
            return std::unexpected(fmt::format("The board contains a value out of range."));
        }
    }

    return {};
}

std::expected<void, std::string> ValidateSumokuPuzzle(const SumokuPuzzleData& puzzle)
{
    const size_t N = puzzle.N;

    // Check if the size of the boxes equals the size of the sums
    if (puzzle.boxes.size() != puzzle.sums.size())
    {
        return std::unexpected(fmt::format("Number of boxes is {}, but the number of sums is {}.", puzzle.boxes.size(), puzzle.sums.size()));
    }

    // Check if no element appears twice
    std::unordered_set<Point, PointHasher> s;
    for (const auto& box : puzzle.boxes)
    {
        if (box.empty())
        {
            return std::unexpected(fmt::format("A box is empty"));
        }

        for (const auto& ele : box)
        {
            if (ele.x >= N || ele.y >= N)
            {
                return std::unexpected(fmt::format("The board contains a value out of range."));
            }

            if (s.contains(ele))
            {
                return std::unexpected(fmt::format("({}, {}) appears twice.", ele.x, ele.y));
            }

            s.insert(ele);
        }
    }

    // Check if the number of elements is correct
    if (s.size() != (N * N))
    {
        return std::unexpected(fmt::format("Only {} element(s) instead of {}.", s.size(),( N * N)));
    }

    // Check if the total sum is correct
    const int expected_sum = ((1 + N) * N / 2) * N;   // There are N rows
    int sum = std::accumulate(std::cbegin(puzzle.sums), std::cend(puzzle.sums), 0);
    if (expected_sum != sum)
    {
        return std::unexpected(fmt::format("Expected sum is {}, but the actual sum is {}.", expected_sum, sum));
    }

    return {};
}

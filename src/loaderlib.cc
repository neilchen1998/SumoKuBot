#include "loader/loaderlib.hpp"

#include <expected> // std::expected
#include <filesystem>   // std::filesystem
#include <fstream>      // std::ifstream
#include <numeric>  // std::accumulate
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
        for (const auto& ele : box)
        {
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

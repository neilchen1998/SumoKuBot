#include <expected> // std::expected
#include <filesystem>   // std::filesystem
#include <fstream>      // std::ifstream
#include <numeric>  // std::accumulate
#include <unordered_set>    // std::unordered_set
#include <vector>    // std::vector

#include <nlohmann/json.hpp>    // nlohmann::json

#include "loaderlib.hpp"

namespace fs = std::filesystem;

/// @brief Validates if the given Sumoku puzzle is valid
/// @param puzzle A Sumoku puzzle
/// @return {} if correct, otherwise an error string
std::expected<void, std::string> ValidateSumokuPuzzle(const SumokuTestData& puzzle)
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

/// @brief Gets the directory of the test data
/// @return Test data directory
std::string GetTestDataPath()
{
    return TEST_DATA_DIR;
}

/// @brief Load all the test cases from the data directory
/// @param dir The directory of the data
/// @return A vector of SumokuTestData
std::vector<SumokuTestData> LoadAllPuzzles(std::string_view dir)
{
    std::vector<SumokuTestData> testCases;

    // Iterate over all the json entries in the directory
    for (const auto& entry : fs::directory_iterator(dir))
    {
        if (entry.path().extension() == ".json")
        {
            std::ifstream file{entry.path()};
            nlohmann::json j;
            file >> j;

            SumokuTestData puzzle = j.get<SumokuTestData>();

            auto result = ValidateSumokuPuzzle(puzzle);

            if (result.has_value())
            {
                testCases.push_back(std::move(puzzle));
            }
            else
            {
                fmt::print(stderr, "Error: {}\n", result.error());
            }
        }
    }

    // Sort the test cases based on the label
    std::sort(std::begin(testCases), std::end(testCases), [](const SumokuTestData& a, const SumokuTestData& b)
    {
        return a.label < b.label;
    });

    return testCases;
}

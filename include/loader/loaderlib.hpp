#ifndef TESTS_COMMON_LOADERLIB_H_
#define TESTS_COMMON_LOADERLIB_H_

#include <expected> // std::expected
#include <filesystem>   // std::filesystem
#include <fstream>      // std::ifstream
#include <numeric>  // std::accumulate
#include <string_view>   // std::string_view
#include <string>   // std::string
#include <unordered_set>    // std::unordered_set
#include <vector>    // std::vector

#include <nlohmann/json.hpp>    // nlohmann::json

#include "board/boardlib.hpp"   // Point
#include "math/mathlib.hpp" // PointHasher

namespace fs = std::filesystem;

/// @brief The Sumoku test data structure
struct SudokuPuzzleData
{
    size_t N;
    std::vector<std::vector<int>> board;
    std::string label;
};

/// @brief The Sumoku test data structure
struct SumokuPuzzleData
{
    size_t N;
    std::vector<std::vector<Point>> boxes;
    std::vector<int> sums;
    std::string label;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SudokuPuzzleData, N, board, label)   // for nlohmann::json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SumokuPuzzleData, N, boxes, sums, label)   // for nlohmann::json

/// @brief Validates if the given Sumoku puzzle is valid
/// @param puzzle A Sumoku puzzle
/// @return {} if correct, otherwise an error string
std::expected<void, std::string> ValidateSumokuPuzzle(const SumokuPuzzleData& puzzle);

/// @brief Validates if the given Sumoku puzzle is valid
/// @param puzzle A Sumoku puzzle
/// @return {} if correct, otherwise an error string
std::expected<void, std::string> ValidateSudokuPuzzle(const SudokuPuzzleData& puzzle);

/// @brief Gets the directory of the test data
/// @return Test data directory
std::string GetTestDataPath();

/// @brief A base trait for puzzle data
/// @tparam T The type of the puzzle data
/// @tparam ValidationFn The validation function for the puzzle
template <typename T, auto ValidationFn>
struct PuzzleTraitsBase
{
    static std::expected<void, std::string> validate(const SumokuPuzzleData& p)
    {
        return ValidationFn(p);
    }

    static const std::string& label(const SumokuPuzzleData& p)
    {
        return p.label;
    }
};

/// @brief Puzzle trait
template <typename T>
struct PuzzleTraits;

/// @brief The explicit specialization of PuzzleTraits for SudokuPuzzleData
template<>
struct PuzzleTraits<SudokuPuzzleData> : PuzzleTraitsBase<SudokuPuzzleData, ValidateSudokuPuzzle>
{

};

/// @brief The explicit specialization of PuzzleTraits for SumokuPuzzleData
template<>
struct PuzzleTraits<SumokuPuzzleData> : PuzzleTraitsBase<SumokuPuzzleData, ValidateSumokuPuzzle>
{

};

/// @brief Loads all the puzzles
/// @tparam T The trait of the puzzle
/// @param dir The directory of the puzzles
/// @return A vector of T
template <typename T>
std::vector<T> LoadAllPuzzles(std::string_view dir)
{
    // Check if the given directory exists
    if (!fs::exists(dir))
    {
        fmt::print(stderr, "Error: '{}' does not exist.\n", dir.data());
        return {};
    }

    // Check if the given argument is a directory
    if (!fs::is_directory(dir))
    {
        fmt::print(stderr, "Error: '{}' is not a directory.\n", dir.data());
        return {};
    }

    std::vector<T> puzzles;

    // Iterate over all the json entries in the directory
    for (const auto& entry : fs::directory_iterator(dir))
    {
        if (entry.path().extension() != ".json" && entry.path().extension() != ".JSON")
        {
            continue;
        }

        std::ifstream ifstrm{entry.path()};

        // Check if the file can be opened
        if (!ifstrm)
        {
            fmt::print(stderr, "Error: Could not open file at '{}'.\n", entry.path().string());
            continue;
        }

        nlohmann::json j;
        ifstrm >> j;

        T puzzle = j.get<T>();

        if (auto result = PuzzleTraits<T>::validate(puzzle); !result)
        {
            fmt::print(stderr, "Error: {}\n", result.error());
            continue;
        }

        puzzles.push_back(std::move(puzzle));
    }

    // Sort the test cases based on the label
    std::ranges::sort(puzzles, [](const T& a, const T& b)
    {
        return a.label < b.label;
    });

    return puzzles;
}

#endif // TESTS_COMMON_LOADERLIB_H_

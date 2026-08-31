#ifndef TESTS_COMMON_LOADERLIB_H_
#define TESTS_COMMON_LOADERLIB_H_

#include <expected>          // std::expected
#include <filesystem>        // std::filesystem
#include <fstream>           // std::ifstream
#include <nlohmann/json.hpp> // nlohmann::json
#include <optional>          // std::optional
#include <ranges>            // std::ranges::sort
#include <string_view>       // std::string_view
#include <vector>            // std::vector

#include "board/boardlib.hpp" // Point

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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SudokuPuzzleData, N, board, label)       // for nlohmann::json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SumokuPuzzleData, N, boxes, sums, label) // for nlohmann::json

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
template <typename T, auto ValidationFn> struct PuzzleTraitsBase
{
    static std::expected<void, std::string> validate(const T& p)
    {
        return ValidationFn(p);
    }

    static const std::string& label(const T& p)
    {
        return p.label;
    }
};

/// @brief Puzzle trait
template <typename T> struct PuzzleTraits;

/// @brief The explicit specialization of PuzzleTraits for SudokuPuzzleData
template <> struct PuzzleTraits<SudokuPuzzleData> : PuzzleTraitsBase<SudokuPuzzleData, ValidateSudokuPuzzle>
{
};

/// @brief The explicit specialization of PuzzleTraits for SumokuPuzzleData
template <> struct PuzzleTraits<SumokuPuzzleData> : PuzzleTraitsBase<SumokuPuzzleData, ValidateSumokuPuzzle>
{
};

/// @brief Loads a puzzle from the given path.
///
/// Reads the puzzle from the specified file, deserializes it into the requested puzzle type
/// and validates the resulting object using PuzzleTraits<T>::validate().
///
/// @tparam T The trait of the puzzle.
/// @param dir The directory of the puzzle.
/// @return The loaded and validated puzzle on success, or std::nullopt on failure.
template <typename T> std::optional<T> LoadPuzzle(std::string_view path)
{
    const fs::path filePath{path};

    if (!fs::exists(filePath))
    {
        fmt::println(stderr, "Error: {} does not exist.", path);
        return std::nullopt;
    }

    std::ifstream ifstrm(filePath);
    if (!ifstrm)
    {
        fmt::println(stderr, "Error: Could not open file at {}.", path);
        return std::nullopt;
    }

    try
    {
        nlohmann::json j;
        ifstrm >> j;

        T puzzle = j.get<T>();

        if (auto result = PuzzleTraits<T>::validate(puzzle); !result)
        {
            fmt::print(stderr, "Error: {}\n", result.error());
            return std::nullopt;
        }

        return puzzle;
    }
    catch (const nlohmann::json::exception& e)
    {
        fmt::print(stderr, "JSON error: {}\n", e.what());
        return std::nullopt;
    }
}

/// @brief Loads all the puzzles
/// @tparam T The trait of the puzzle
/// @param dir The directory of the puzzles
/// @return A vector of T
template <typename T> std::vector<T> LoadAllPuzzles(std::string_view dir)
{
    // Check if the given directory exists
    if (!fs::exists(dir))
    {
        fmt::print(stderr, "Error: '{}' does not exist.\n", dir);
        return {};
    }

    // Check if the given argument is a directory
    if (!fs::is_directory(dir))
    {
        fmt::print(stderr, "Error: '{}' is not a directory.\n", dir);
        return {};
    }

    std::vector<T> puzzles;

    // Iterate over all the json entries in the directory
    for (const auto& entry : fs::directory_iterator(dir))
    {
        if (entry.path().extension() == ".json" || entry.path().extension() == ".JSON")
        {
            if (auto puzzle = LoadPuzzle<T>(entry.path().string()); puzzle)
            {
                puzzles.push_back(std::move(*puzzle));
            }
        }
    }

    // Sort the test cases based on the label
    std::ranges::sort(puzzles, [](const T& a, const T& b) { return a.label < b.label; });

    return puzzles;
}

#endif // TESTS_COMMON_LOADERLIB_H_

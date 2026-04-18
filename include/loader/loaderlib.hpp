#ifndef TESTS_COMMON_LOADERLIB_H_
#define TESTS_COMMON_LOADERLIB_H_

#include <string_view>   // std::string_view
#include <string>   // std::string
#include <vector>   // std::vector

#include "board/boardlib.hpp"   // Point

/// @brief The Sumoku test data structure
struct SumokuPuzzleData
{
    size_t N;
    std::vector<std::vector<Point>> boxes;
    std::vector<int> sums;
    std::string label;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SumokuPuzzleData, N, boxes, sums, label)   // for nlohmann::json

/// @brief Gets the directory of the test data
/// @return Test data directory
std::string GetTestDataPath();

/// @brief Loads all the test cases from the data directory
/// @param dir The directory of the data
/// @return A vector of SumokuPuzzleData
std::vector<SumokuPuzzleData> LoadAllPuzzles(std::string_view dir);

/// @brief Loads the puzzle from a JSON file
/// @param filePath The JSON file path
/// @return SumokuPuzzleData
SumokuPuzzleData LoadPuzzle(std::string_view filePath);

#endif // TESTS_COMMON_LOADERLIB_H_

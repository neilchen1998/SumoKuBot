#include <filesystem>   // std::filesystem
#include <fstream>      // std::ifstream
#include <vector>    // std::vector

#include <nlohmann/json.hpp>    // nlohmann::json

#include "loaderlib.hpp"

namespace fs = std::filesystem;

/// @brief Gets the directory of the test data
/// @return Test data directory
std::string GetTestDataPath()
{
    // Gets the current path of this file
    // __FILE__ is the path to the current file
    fs::path source_file = __FILE__;

    return (source_file.parent_path() / "data").string();
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
            testCases.push_back(puzzle);
        }
    }

    return testCases;
}

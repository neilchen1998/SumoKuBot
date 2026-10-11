#include <chrono>      // std::chrono::milliseconds
#include <cstdlib>     // EXIT_FAILURE, EXIT_SUCCESS
#include <expected>    // std::expected
#include <filesystem>  // std::filesystem
#include <fmt/core.h>  // fmt::format
#include <fstream>     // std::ofstream
#include <nanobench.h> // ankerl::nanobench::Bench
#include <string>      // std::getline, std::string
#include <vector>      // std::vector

#include "loader/loaderlib.hpp"                           // GetTestDataPath, LoadAllPuzzles<>
#include "solvers/sudoku/killersudokumrvsolver.hpp"       // killer_sudoku::KillerSudokuMRVSolver
#include "solvers/sumoku/sumokubacktrackingsolver.hpp"    // sumoku::SumokuBacktracking
#include "solvers/sumoku/sumokubitmaskorderingsolver.hpp" // sumoku::SumokuBacktracking
#include "solvers/sumoku/sumokuorderingsolver.hpp"        // sumoku::SumokuBacktracking

namespace fs = std::filesystem;

namespace
{
/// @brief Opens a file from the given file path.
///
/// @param filePath The file path.
/// @return An std::expected containing the opened file stream on success, or an error message on failure.
std::expected<std::fstream, std::string> OpenFile(const fs::path& filePath)
{
    std::fstream file(filePath);
    if (!file)
    {
        return std::unexpected(fmt::format("Failed to open {}", filePath.string()));
    }

    return file;
}
} // namespace

int main()
{
    // Output directory
    const fs::path outputDir = "results";
    fs::create_directories(outputDir);

    // Sumoku
    {
        const fs::path filename {outputDir / "solver-results.csv"};
        std::ofstream file {filename};

        // Load the puzzles
        const std::string folder = GetTestDataPath() + "/sumoku/solvable";
        const std::vector<SumokuPuzzleData> all_puzzles = LoadAllPuzzles<SumokuPuzzleData>(folder);

        bool writeHeader = true;

        for (const auto& p : all_puzzles)
        {
            ankerl::nanobench::Bench bench;
            bench.title(fmt::format("Sumoku Solver Comparison #{}", p.label))
                .run("backtracking",
                     [&] {
                         sumoku::SumokuBacktrackingSolver s {p.N, p.boxes, p.sums};

                         s.Solve();
                         ankerl::nanobench::doNotOptimizeAway(s);
                     })
                .run("ordering",
                     [&] {
                         sumoku::SumokuOrderingSolver s {p.N, p.boxes, p.sums};

                         s.Solve();
                         ankerl::nanobench::doNotOptimizeAway(s);
                     })
                .run("bitmask ordering",
                     [&] {
                         sumoku::SumokuBitMaskOrderingSolver s {p.N, p.boxes, p.sums};

                         s.Solve();
                         ankerl::nanobench::doNotOptimizeAway(s);
                     })
                .run("MRV", [&] {
                    sumoku::SumokuMRVSolver s {p.N, p.boxes, p.sums};

                    s.Solve();
                    ankerl::nanobench::doNotOptimizeAway(s);
                });

            // Render the results to the buffer for further process
            std::ostringstream buffer;
            bench.render(ankerl::nanobench::templates::csv(), buffer);

            // Store the buffer as input string stream
            std::istringstream input(buffer.str());
            std::string line;

            // Go through each line of the buffer
            while (std::getline(input, line))
            {
                if (line.empty())
                {
                    continue;
                }

                // Only write the title once
                if (writeHeader)
                {
                    file << line << "\n";
                    writeHeader = false;
                }
                else if (!line.starts_with("\"title\";"))
                {
                    file << line << "\n";
                }
            }
        }

        // Check if the output file exists
        if (auto result = OpenFile(filename); !result)
        {
            fmt::println(stderr, "{}", result.error());
            return EXIT_FAILURE;
        }
    }

    // Killer Sudoku
    {
        const fs::path filename {outputDir / "killer-sudoku-results.csv"};
        std::ofstream file(filename);

        ankerl::nanobench::Bench bench;
        bench.title("Killer Sudoku").timeUnit(std::chrono::milliseconds(1), "ms"); // uses ms as the unit

        // Load the puzzles
        const std::string folder = GetTestDataPath() + "/killer_sudoku/solvable";
        const std::vector<SumokuPuzzleData> all_puzzles = LoadAllPuzzles<SumokuPuzzleData>(folder);

        for (const auto& p : all_puzzles)
        {
            bench.run(fmt::format("MRV - #{}", p.label), [&] {
                killer_sudoku::KillerSudokuMRVSolver s {p.N, p.boxes, p.sums};

                s.Solve();
                ankerl::nanobench::doNotOptimizeAway(s);
            });
        }

        bench.render(ankerl::nanobench::templates::csv(), file);

        // Check if the output file exists
        if (auto result = OpenFile(filename); !result)
        {
            fmt::println(stderr, "{}", result.error());
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

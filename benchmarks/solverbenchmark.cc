#include <chrono>      // std::chrono::milliseconds
#include <filesystem>  // std::filesystem
#include <fmt/core.h>  // fmt::format
#include <fstream>     // std::ofstream
#include <nanobench.h> // ankerl::nanobench::Bench
#include <vector>      // std::vector

#include "loader/loaderlib.hpp"                           // GetTestDataPath, LoadAllPuzzles<>
#include "solvers/sudoku/killersudokumrvsolver.hpp"       // killer_sudoku::KillerSudokuMRVSolver
#include "solvers/sumoku/sumokubacktrackingsolver.hpp"    // sumoku::SumokuBacktracking
#include "solvers/sumoku/sumokubitmaskorderingsolver.hpp" // sumoku::SumokuBacktracking
#include "solvers/sumoku/sumokuorderingsolver.hpp"        // sumoku::SumokuBacktracking

namespace fs = std::filesystem;

int main()
{
    // Output directory
    const fs::path outputDir = "results";
    fs::create_directories(outputDir);

    // Sumoku
    {
        const fs::path filename {outputDir / "solver-results.csv"};
        std::ofstream file(filename);
        if (!file.is_open())
        {
            throw std::runtime_error(fmt::format("Failed to open {}", filename.string()));
        }

        ankerl::nanobench::Bench bench;

        // Load the puzzles
        const std::string folder = GetTestDataPath() + "/sumoku/solvable";
        const std::vector<SumokuPuzzleData> all_puzzles = LoadAllPuzzles<SumokuPuzzleData>(folder);

        for (const auto& p : all_puzzles)
        {
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
        }

        bench.render(ankerl::nanobench::templates::csv(), file);
    }

    // Killer Sudoku
    {
        const fs::path filename {outputDir / "killer-sudoku-results.csv"};
        std::ofstream file(filename);
        if (!file.is_open())
        {
            throw std::runtime_error(fmt::format("Failed to open {}", filename.string()));
        }

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
    }
}

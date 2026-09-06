#include <chrono>      // std::chrono::milliseconds
#include <fmt/core.h>  // fmt::format
#include <fstream>     // std::ofstream
#include <nanobench.h> // ankerl::nanobench::Bench
#include <vector>      // std::vector

#include "loader/loaderlib.hpp"                           // GetTestDataPath, LoadAllPuzzles<>
#include "solvers/sudoku/killersudokumrvsolver.hpp"       // killer_sudoku::KillerSudokuMRVSolver
#include "solvers/sumoku/sumokubacktrackingsolver.hpp"    // sumoku::SumokuBacktracking
#include "solvers/sumoku/sumokubitmaskorderingsolver.hpp" // sumoku::SumokuBacktracking
#include "solvers/sumoku/sumokuorderingsolver.hpp"        // sumoku::SumokuBacktracking

int main()
{
    std::ofstream file("./build/benchmarks/solver-results.csv");
    ankerl::nanobench::Bench bench;

    // Load the puzzles
    const std::string folder = GetTestDataPath();
    const std::vector<SumokuPuzzleData> all_puzzles = LoadAllPuzzles<SumokuPuzzleData>(folder);

    for (const auto& p : all_puzzles)
    {
        bench.title(fmt::format("Sumoku Solver Comparison #{}", p.label))
            .run("backtracking", [&]
        {
            sumoku::SumokuBacktrackingSolver s {p.N, p.boxes, p.sums};

            s.Solve();
            ankerl::nanobench::doNotOptimizeAway(s);
        })
            .run("ordering", [&]
        {
            sumoku::SumokuOrderingSolver s {p.N, p.boxes, p.sums};

            s.Solve();
            ankerl::nanobench::doNotOptimizeAway(s);
        })
            .run("bitmask ordering", [&]
        {
            sumoku::SumokuBitMaskOrderingSolver s {p.N, p.boxes, p.sums};

            s.Solve();
            ankerl::nanobench::doNotOptimizeAway(s);
        })
            .run("MRV", [&]
        {
            sumoku::SumokuMRVSolver s {p.N, p.boxes, p.sums};

            s.Solve();
            ankerl::nanobench::doNotOptimizeAway(s);
        });
    }

    // Killer Sudoku
    {
        std::ofstream killerSudokuFile("./build/benchmarks/killer-sudoku-results.csv");
        ankerl::nanobench::Bench bench;
        bench.title("Killer Sudoku")
            .timeUnit(std::chrono::milliseconds(1), "ms");  // uses ms as the unit

        // Load the puzzles
        const std::string folder = GetTestDataPath() + "/killer_sudoku";
        const std::vector<SumokuPuzzleData> all_puzzles = LoadAllPuzzles<SumokuPuzzleData>(folder);

        for (const auto& p : all_puzzles)
        {
            bench.run(fmt::format("MRV - #{}", p.label), [&]
            {
                killer_sudoku::KillerSudokuMRVSolver s {p.N, p.boxes, p.sums};

                s.Solve();
                ankerl::nanobench::doNotOptimizeAway(s);
            });
        }

        bench.render(ankerl::nanobench::templates::csv(), killerSudokuFile);
    }
}

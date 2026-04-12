
#include <chrono>   // std::chrono::milliseconds
#include <fstream>  // std::ofstream
#include <nanobench.h>  // ankerl::nanobench::Bench
#include <vector>   // std::vector

#include <fmt/core.h>   // fmt::format

#include "solver/solverlib.hpp"   // SumokuSolver, SumokuOrdering, etc.

#include "shared/loaderlib.hpp" // GetTestDataPath, LoadAllPuzzles

int main()
{
    std::ofstream file("./build/benchmarks/solver-results.csv");
    ankerl::nanobench::Bench bench;

    // Load the puzzles
    const std::string folder = GetTestDataPath();
    const std::vector<SumokuTestData> all_puzzles = LoadAllPuzzles(folder);

    for (const auto& p : all_puzzles)
    {
        bench.title(fmt::format("Sumoku Solver Comparison #{}", p.label))
            .run("traditional", [&]
        {
            solver::SumokuSolver s {p.N, p.boxes, p.sums};

            s.Solve();
            ankerl::nanobench::doNotOptimizeAway(s);
        })
            .run("MRV", [&]
        {
            solver::SumokuMRV s {p.N, p.boxes, p.sums};

            s.Solve();
            ankerl::nanobench::doNotOptimizeAway(s);
        });
    }

    {
        // Killer Sudoku
        std::ofstream killerSudokuFile("./build/benchmarks/killer-sudoku-results.csv");
        ankerl::nanobench::Bench bench;
        bench.title("Killer Sudoku")
            .timeUnit(std::chrono::milliseconds(1), "ms");  // uses ms as the unit

        // Load the puzzles
        const std::string folder = GetTestDataPath() + "/killer_sudoku";
        const std::vector<SumokuTestData> all_puzzles = LoadAllPuzzles(folder);

        for (const auto& p : all_puzzles)
        {
            bench.run(fmt::format("MRV - #{}", p.label), [&]
            {
                solver::KillerSudokuMRV s {p.N, p.boxes, p.sums};

                s.Solve();
                ankerl::nanobench::doNotOptimizeAway(s);
            });
        }

        bench.render(ankerl::nanobench::templates::csv(), killerSudokuFile);
    }
}

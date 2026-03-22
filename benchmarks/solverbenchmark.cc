
#include <chrono>   // std::chrono::milliseconds
#include <fstream>  // std::ofstream
#include <nanobench.h>  // ankerl::nanobench::Bench
#include <vector>   // std::vector

#include <fmt/core.h>   // fmt::format

#include "solver/solverlib.hpp"   // SumokuSolver, SumokuOrdering, etc.

#include "loaderlib.hpp"

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
}

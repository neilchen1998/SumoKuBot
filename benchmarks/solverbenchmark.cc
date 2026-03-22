
#include <chrono>   // std::chrono::milliseconds
#include <vector>   // std::vector
#include <nanobench.h>  // ankerl::nanobench::Bench
#include <fstream>  // std::ofstream

#include "solver/solverlib.hpp"   // SumokuSolver, SumokuOrdering, etc.

#include "loaderlib.hpp"

int main()
{
    std::ofstream file("./build/benchmarks/solver-results.csv");
    ankerl::nanobench::Bench bench;
    bench.timeUnit(std::chrono::milliseconds(1), "ms"); // change to ms instead of the default ns

    static std::string folder = GetTestDataPath();
    static std::vector<SumokuTestData> all_puzzles = LoadAllPuzzles(folder);

    constexpr size_t N = 9;

    const auto boxes1 = all_puzzles[0].boxes;
    const auto sums1 = all_puzzles[0].sums;

    const auto boxes2 = all_puzzles[1].boxes;
    const auto sums2 = all_puzzles[1].sums;

    bench.title("Sumoku Solver Comparison #1")
        .run("traditional", [&]
    {
        solver::SumokuSolver s {N, boxes1, sums1};

        s.Solve();
        ankerl::nanobench::doNotOptimizeAway(s);
    })
        .run("traditional w/ bit mask", [&]
    {
        solver::SumokuSolverWithBitMask s {N, boxes1, sums1};

        s.Solve();
        ankerl::nanobench::doNotOptimizeAway(s);
    })
        .run("ordering", [&]
    {
        solver::SumokuOrdering s {N, boxes1, sums1};

        s.Solve();
        ankerl::nanobench::doNotOptimizeAway(s);
    })
        .run("ordering w/ bit mask", [&]
    {
        solver::SumokuOrderingWithBitMask s {N, boxes1, sums1};

        s.Solve();
        ankerl::nanobench::doNotOptimizeAway(s);
    })
        .run("MRV", [&]
    {
        solver::SumokuMRV s {N, boxes1, sums1};

        s.Solve();
        ankerl::nanobench::doNotOptimizeAway(s);
    });

    bench.title("Sumoku Solver Comparison #2")
        .run("traditional", [&]
    {
        solver::SumokuSolver s {N, boxes2, sums2};

        s.Solve();
        ankerl::nanobench::doNotOptimizeAway(s);
    })
        .run("traditional with bit mask", [&]
    {
        solver::SumokuSolverWithBitMask s {N, boxes2, sums2};

        s.Solve();
        ankerl::nanobench::doNotOptimizeAway(s);
    })
        .run("ordering", [&]
    {
        solver::SumokuOrdering s {N, boxes2, sums2};

        s.Solve();
        ankerl::nanobench::doNotOptimizeAway(s);
    })
        .run("ordering w/ bit mask", [&]
    {
        solver::SumokuOrderingWithBitMask s {N, boxes2, sums2};

        s.Solve();
        ankerl::nanobench::doNotOptimizeAway(s);
    })
        .run("MRV", [&]
    {
        solver::SumokuMRV s {N, boxes1, sums1};

        s.Solve();
        ankerl::nanobench::doNotOptimizeAway(s);
    });
}

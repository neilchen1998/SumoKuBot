
#include <chrono>   // std::chrono::milliseconds
#include <vector>   // std::vector
#include <nanobench.h>  // ankerl::nanobench::Bench
#include <fstream>  // std::ofstream

#include "solver/solverlib.hpp"   // SumokuSolver, SumokuOrdering, etc.

int main()
{
    std::ofstream file("./build/benchmarks/solver-results.csv");
    ankerl::nanobench::Bench bench;
    bench.timeUnit(std::chrono::milliseconds(1), "ms"); // change to ms instead of the default ns

    constexpr size_t N = 9;
    const std::vector<std::vector<Point>> boxes1 {
        {{0, 0}, {0, 1}}, {{0, 2}, {1, 2}}, {{0 ,3}, {0, 4}}, {{0, 5}, {1, 5}}, {{0, 6}, {0, 7}}, {{0, 8}, {1, 8}},
        {{1, 0}, {1, 1}}, {{1, 3}, {1, 4}}, {{1, 6}, {1, 7}},
        {{2, 0}, {3, 0}}, {{2, 1}, {3, 1}}, {{2, 2}, {3, 2}}, {{2, 3}, {3, 3}}, {{2, 4}, {3, 4}}, {{2, 5}, {2, 6}}, {{2, 7}, {3, 7}}, {{2, 8}, {3, 8}},
        {{3, 5}, {3, 6}},
        {{4, 0}, {4, 1}}, {{4, 2}, {4, 3}}, {{4, 4}, {4, 5}}, {{4, 6}, {5, 6}}, {{4, 7}, {4, 8}},
        {{5, 0}, {5, 1}}, {{5, 2}, {5, 3}}, {{5, 4}, {5, 5}}, {{5, 7}, {5, 8}},
        {{6, 0}, {6, 1}}, {{6, 2}, {6, 3}}, {{6, 4}, {6, 5}}, {{6, 6}, {7, 6}}, {{6, 7}, {6, 8}},
        {{7, 0}, {8, 0}}, {{7, 1}, {7, 2}}, {{7, 3}, {8, 3}}, {{7, 4}, {7, 5}}, {{7, 7}, {8, 7}}, {{7, 8}, {8, 8}},
        {{8, 1}, {8, 2}}, {{8, 4}, {8, 5}, {8, 6}}
    };
    const std::vector<int> sums1 {
        15, 13, 10, 5, 8, 8,
        12, 6, 13,
        15, 6, 4, 5, 13, 12, 11, 13,
        11,
        9, 11, 13, 3, 10,
        7, 13, 17, 7,
        10, 7, 10, 12, 11,
        9, 11, 14, 11, 10, 8,
        9, 13
    };

    const std::vector<std::vector<Point>> boxes2 {
        {{0, 0}, {1, 0}, {2, 0}}, {{0, 1}, {1, 1}}, {{0, 2}, {1, 2}}, {{0, 3}, {1, 3}}, {{0, 4}, {1, 4}}, {{0, 5}, {0, 6}}, {{0, 7}, {0, 8}},
        {{1, 5}, {2, 5}}, {{1, 6}, {2, 6}}, {{1, 7}, {1, 8}},
        {{2, 1}, {2, 2}}, {{2, 3}, {2, 4}}, {{2, 7}, {3, 7}, {4, 7}}, {{2, 8}, {3, 8}},
        {{3, 0}, {3, 1}}, {{3, 2}, {3, 3}}, {{3, 4}, {3, 5}}, {{3, 6}, {4, 6}},
        {{4, 0}, {5, 0}, {6, 0}}, {{4, 1}, {5, 1}}, {{4, 2}, {5, 2}}, {{4, 3}, {4, 4}}, {{4, 5}, {5, 5}}, {{4, 8}, {5, 8}},
        {{5, 3}, {5, 4}}, {{5, 6}, {5, 7}},
        {{6, 1}, {6, 2}}, {{6, 3}, {6, 4}}, {{6, 5}, {6, 6}}, {{6, 7}, {7, 7}}, {{6, 8}, {7, 8}},
        {{7, 0}, {7, 1}}, {{7, 2}, {8, 2}}, {{7, 3}, {8, 3}}, {{7, 4}, {8, 4}}, {{7, 5}, {7, 6}},
        {{8, 0}, {8, 1}}, {{8, 5}, {8, 6}}, {{8, 7}, {8, 8}}
    };
    const std::vector<int> sums2 {
        18, 4, 8, 10, 13, 13, 17,
        5, 11, 7,
        5, 15, 17, 11,
        10, 6, 10, 17,
        11, 12, 12, 11, 5, 8,
        12, 5,
        13, 9, 11, 11, 11,
        17, 10, 6, 11, 8,
        10, 11, 4
    };

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

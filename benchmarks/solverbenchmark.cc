
#include <chrono>   // std::chrono::milliseconds
#include <vector>   // std::vector
#include <nanobench.h>  // ankerl::nanobench::Bench
#include <fstream>  // std::ofstream

#include "solver/solverlib.hpp"   //

int main()
{
    std::ofstream file("./build/benchmarks/solver-results.csv");
    ankerl::nanobench::Bench bench;
    bench.timeUnit(std::chrono::milliseconds(1), "ms"); // change to ms instead of the default ns

    constexpr size_t N = 9;
    const std::vector<std::vector<Point>> boxes {
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
    const std::vector<int> sums {
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

    bench.title("Sumoku Solver Comparison")
        .minEpochIterations(10)
        .run("traditional", [&]
    {
        solver::SumokuSolver s {N, boxes, sums};

        s.Solve();
        ankerl::nanobench::doNotOptimizeAway(s);
    })
        .run("ordering", [&]
    {
        solver::SumokuOrdering s {N, boxes, sums};

        s.Solve();
        ankerl::nanobench::doNotOptimizeAway(s);
    });

}

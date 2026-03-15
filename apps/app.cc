#include <chrono>   // std::chrono::high_resolution_clock
#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <vector>  // std::vector

#include <fmt/core.h>   // fmt::print

#include "board/boardlib.hpp"
#include "solver/solverlib.hpp"

int main(int argc, char* argv[])
{
    constexpr size_t N = 9;

    const std::vector<std::vector<Point>> boxes {
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
    const std::vector<int> sums {
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

    solver::SumokuSolver s {N, boxes, sums};

    auto start = std::chrono::high_resolution_clock::now();
    s.Solve();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    fmt::print("Elapsed time: {:.3f} ms\n", elapsed.count());

    return EXIT_SUCCESS;
}

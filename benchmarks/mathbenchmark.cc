#include <array>   // std::array
#include <fstream>  // std::ofstream
#include <nanobench.h>  // ankerl::nanobench::Bench
#include <random>   // std::mt19937_64

#include "board/boardlib.hpp"   // Point
#include "math/mathlib.hpp"        // PointHasher
#include "shared/boostmathlib.hpp" // BoostPointHasher

int main()
{
    std::ofstream file("./build/benchmarks/board-results.csv");
    ankerl::nanobench::Bench bench;

    // Hashers
    PointHasher hasher {};
    BoostPointHasher boostHasher {};

    std::array<Point, 1024> points;    // 1024 is a power of 2
    std::mt19937_64 rng(42);

    // Produce random values [1, 9]
    std::uniform_int_distribution<> distrib(0, 8);

    for (Point& p : points)
    {
        p.x = distrib(rng);
        p.y = distrib(rng);
    }

    size_t i = 0;
    bench.title("Hashing")
        .run("PointerHasher", [&]
    {
        auto& p = points[i++ & 1023];   // equivalent to (i++) % 1024
        auto h = hasher(p);
        ankerl::nanobench::doNotOptimizeAway(h);
    });

    i = 0;
    bench.run("PointHasher w/ boost", [&]
    {
        auto& p = points[i++ & 1023];   // equivalent to (i++) % 1024
        auto h = boostHasher(p);
        ankerl::nanobench::doNotOptimizeAway(h);
    });
}

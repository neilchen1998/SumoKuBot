#include <chrono>   // std::chrono::high_resolution_clock
#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <string_view>
#include <string>
#include <utility>  // std::pair
#include <vector>  // std::vector

#include <boost/program_options.hpp>    // boost::program_options
#include <CLI/CLI.hpp>  // CLI::App, CLI::ParseError
#include <fmt/core.h>   // fmt::print
#include <fmt/ostream.h>    // fmt::streamed

#include "board/boardlib.hpp"
#include "solver/solverlib.hpp"

enum class Solvers { SumokuMRV, OtherType };

/// @brief A bi-directional map for solvers
static const std::vector<std::pair<Solvers, std::string_view>> SolverMapping
{
    {Solvers::SumokuMRV, "SumokuMRV"},
    {Solvers::OtherType, "OtherType"}
};

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

/// @brief Overloads the stream insertion operator to convert Solvers enum value to string
/// @param os The output stream
/// @param s The solver enum value
/// @return A reference to the output stream
std::ostream& operator<<(std::ostream& os, const Solvers& s)
{
    for (const auto& [solver, name] : SolverMapping)
    {
        if (solver == s)    return os << name;
    }

    return os << "Unknown";
}

/// @brief Overloads the stream extraction operator to parse strings into Solvers enum value
/// @param is The input stream
/// @param s The solver enum type
/// @return A reference to the input stream
std::istream& operator>>(std::istream& is, Solvers& s)
{
    std::string input;
    if (is >> input)
    {
        for (const auto& [solver, name] : SolverMapping)
        {
            if (name == input)
            {
                s = solver;
                return is;
            }
        }

        is.setstate(std::ios::failbit);
    }

    return is;
}

int main(int argc, char* argv[])
{
    CLI::App app {"Options:"};

    std::string solver {"SumokuMRV"};
    bool version = false;
    bool verbose = false;
    bool benchmark = false;

    app.add_option("-s,--solver", solver, "Solver type")->capture_default_str();
    app.add_flag("-v,--verbose", verbose, "Enable verbose mode");
    app.add_flag("-b,--benchmark", benchmark, "Show benchmark result");

    // Check if the user inputs are valid
    try
    {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e)
    {
        return app.exit(e);
    }

    if (verbose)
    {
        fmt::print("Solver selected: {}\n", solver);
    }

    solver::SumokuMRV s {N, boxes, sums};

    auto start = std::chrono::high_resolution_clock::now();
    s.Solve();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    if (benchmark)
    {
        fmt::print("Elapsed time: {:.3f} ms\n", elapsed.count());
    }

    return EXIT_SUCCESS;
}

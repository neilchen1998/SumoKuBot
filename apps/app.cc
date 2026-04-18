#include <chrono>   // std::chrono::high_resolution_clock
#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <filesystem>   // std::filesystem
#include <map>  // std::map
#include <string>   // std::string
#include <vector>  // std::vector

#include <boost/program_options.hpp>    // boost::program_options
#include <CLI/CLI.hpp>  // CLI::App, CLI::ParseError
#include <fmt/core.h>   // fmt::print
#include <fmt/ostream.h>    // fmt::streamed
#include <fmt/std.h>    // fmt::println for filesystem

#include "board/boardlib.hpp"
#include "loader/loaderlib.hpp"
#include "solver/solverlib.hpp"
#include "version.h"    // SUMOKUBOT_PROJECT_NAME, SUMOKUBOT_PROJECT_VERSION

namespace fs = std::filesystem;

/// @brief The solver type
enum class SolverType { SumokuSolver, SumokuMRV, SumokuOrdering };

/// @brief A map for solvers
static const std::map<std::string, SolverType> solverMap
{
    {"Basic", SolverType::SumokuSolver},
    {"SumokuMRV", SolverType::SumokuMRV},
    {"SumokuOrdering", SolverType::SumokuOrdering},
};

/// @brief Overloads the stream insertion operator to convert Solvers enum value to string
/// @param os The output stream
/// @param s The solver enum value
/// @return A reference to the output stream
std::ostream& operator<<(std::ostream& os, const SolverType& s)
{
    for (const auto& [name, solver] : solverMap)
    {
        if (solver == s)    return os << name;
    }

    return os << "Unknown";
}

int main(int argc, char* argv[])
{
    CLI::App app {"Options:"};
    app.name(SUMOKUBOT_PROJECT_NAME);

    SolverType solver {SolverType::SumokuMRV};
    fs::path filePath {"./tests/data/killer_sudoku/puzzle_p4.json"};
    fs::path dirPath;
    bool verbose = false;
    bool benchmark = false;

    // The solver
    app.add_option("-s,--solver", solver, "The solver type")
        ->transform(CLI::CheckedTransformer(solverMap, CLI::ignore_case))
        ->option_text("{Basic, SumokuMRV, SumokuOrdering}")
        ->capture_default_str();

    // Souce of the puzzle (directory or file)
    auto group = app.add_option_group("Puzzle source", "Specify either a file or directory");
    group->add_option("-f,--file", filePath, "Path to the puzzle file")
       ->check(CLI::ExistingFile);

    group->add_option("-d,--dir", dirPath, "Path to the puzzle directory")
       ->check(CLI::ExistingDirectory);

    group->require_option(0, 1);    // at most one option from this group

    // Verbose
    app.add_flag("--verbose", verbose, "Enable verbose mode");

    // Benchmark
    app.add_flag("-b,--benchmark", benchmark, "Show benchmark result");

    // Version
    std::string versionInfo = fmt::format("{}: {}", app.get_name(), SUMOKUBOT_PROJECT_VERSION);
    app.set_version_flag("-v,--version", versionInfo);

    // Check if the user inputs are valid
    try
    {
        app.parse(argc, argv);

        if (!filePath.empty() && verbose)
        {
            fmt::println("Loading puzzle from: '{}'.", filePath);
            fmt::println("File size: {} bytes.", fs::file_size(filePath));
        }
        else if (!dirPath.empty() && verbose)
        {
            fmt::println("Loading puzzle from: '{}'.", dirPath);
        }
    }
    catch (const CLI::ParseError& e)
    {
        return app.exit(e);
    }

    // Print out the solver
    if (verbose)
    {
        fmt::println("Solver selected: {}", fmt::streamed(solver));
    }

    // Load the puzzle(s) to a vector
    std::vector<SumokuPuzzleData> puzzles;
    if (!dirPath.empty())
    {
        puzzles = LoadAllPuzzles(dirPath.string());
    }
    else
    {
        puzzles.push_back(LoadPuzzle(filePath.string()));
    }

    // Loop through all puzzles and solve them
    for (const auto& p : puzzles)
    {
        solver::SumokuMRV s {p.N, p.boxes, p.sums};

        auto start = std::chrono::high_resolution_clock::now();
        s.Solve();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = (end - start);

        if (auto board = s.GetSolution())
        {
            fmt::println("*** Result of Puzzle #{} ***", p.label);
            fmt::println("");
            PrintBoard(*board);
        }
        else
        {
            fmt::println("Failed to solve!");
            continue;
        }

        if (benchmark)
        {
            fmt::println("Solved in: {:.3f} ms", elapsed.count());
        }

        fmt::println("");
    }

    return EXIT_SUCCESS;
}

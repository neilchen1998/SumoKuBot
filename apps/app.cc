#include <CLI/CLI.hpp>     // CLI::App, CLI::ParseError
#include <array>           // std::array
#include <chrono>          // std::chrono::high_resolution_clock
#include <cstdint>         // std::uint8_t
#include <cstdlib>         // EXIT_SUCCESS, EXIT_FAILURE
#include <filesystem>      // std::filesystem
#include <fmt/core.h>      // fmt::print
#include <fmt/ostream.h>   // fmt::streamed
#include <fmt/std.h>       // fmt::println for filesystem
#include <spdlog/spdlog.h> // spdlog::set_level, spdlog::info
#include <string>          // std::string
#include <string_view>     // std::string_view
#include <vector>          // std::vector

#include "board/boardlib.hpp"   // PrintBoard
#include "loader/loaderlib.hpp" // LoadAllPuzzles, LoadPuzzle
#include "solvers/sumoku/sumokubacktrackingsolver.hpp" // sumoku::SumokuBacktracking
#include "solvers/sumoku/sumokubitmaskorderingsolver.hpp" // sumoku::SumokuBacktracking
#include "solvers/sumoku/sumokuorderingsolver.hpp" // sumoku::SumokuBacktracking
#include "version.h"            // SUMOKUBOT_PROJECT_NAME, SUMOKUBOT_PROJECT_VERSION

namespace fs = std::filesystem;

/// @brief The solver type
enum class SolverType : std::uint8_t // packs this enum inside a single byte type
{
    SumokuSolver,
    SumokuMRV,
    SumokuOrdering
};

/// @brief A map for all the solver types
constexpr std::array<std::pair<std::string_view, SolverType>, 3> solverMap {{
    {"Basic", SolverType::SumokuSolver},
    {"SumokuMRV", SolverType::SumokuMRV},
    {"SumokuOrdering", SolverType::SumokuOrdering},
}};

/// @brief Overloads the stream insertion operator to convert Solvers enum value to string
/// @param os The output stream
/// @param s The solver type enum value
/// @return A reference to the output stream
std::ostream& operator<<(std::ostream& os, const SolverType& s)
{
    for (const auto& [name, solver] : solverMap)
    {
        if (solver == s)
        {
            return os << name;
        }
    }

    return os << "Unknown";
}

/// @brief Solves a puzzle and prints the result
/// @tparam T The solver type
/// @param s The solver instance
/// @param puzzle The puzzle data
/// @param benchmark True if the user wants to print the timing info
template <typename T> void RunSolver(T& s, const SumokuPuzzleData& puzzle, bool benchmark)
{
    auto start = std::chrono::high_resolution_clock::now();
    s.Solve();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = (end - start);

    if (auto board = s.GetSolution())
    {
        fmt::println("*** Result of Puzzle #{} ***", puzzle.label);
        fmt::println("");
        PrintBoard(*board);
    }
    else
    {
        fmt::println("Failed to solve!");
        return;
    }

    if (benchmark)
    {
        fmt::println("Solved in: {:.3f} ms", elapsed.count());
    }

    fmt::println("");
}

int main(int argc, char* argv[])
{
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif

    spdlog::info("Application started");

    CLI::App app {"Options:"};
    app.name(SUMOKUBOT_PROJECT_NAME);

    SolverType solverType {SolverType::SumokuMRV};
    fs::path filePath;
    fs::path dirPath;
    bool verbose = false;
    bool benchmark = false;

    // The solver
    app.add_option("-s,--solver", solverType, "The solver type")
        ->transform(CLI::CheckedTransformer(solverMap))
        ->option_text("{Basic, SumokuMRV, SumokuOrdering}")
        ->capture_default_str();

    // Souce of the puzzle (directory or file)
    auto group = app.add_option_group("Puzzle source", "Specify either a file or directory");
    group->add_option("-f,--file", filePath, "Path to the puzzle file")->check(CLI::ExistingFile);

    group->add_option("-d,--dir", dirPath, "Path to the puzzle directory")->check(CLI::ExistingDirectory);

    group->require_option(0, 1); // at most one option from this group

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

        if (filePath.empty() && dirPath.empty())
        {
            filePath = fs::path {GetTestDataPath()} / "puzzle_p4.json";

            spdlog::debug("No puzzle source specified; using default puzzle: '{}'", filePath.string());
        }

        if (!filePath.empty())
        {
            spdlog::debug("Puzzle file selected: '{}'", filePath.string());

            if (verbose)
            {
                fmt::println("Loading puzzle from: '{}'.", filePath);
                fmt::println("File size: {} bytes.", fs::file_size(filePath));
            }
        }
        else if (!dirPath.empty())
        {
            spdlog::debug("Puzzle directory selected: '{}'", dirPath.string());

            if (verbose)
            {
                fmt::println("Loading puzzles from: '{}'.", dirPath);
            }
        }
    }
    catch (const CLI::ParseError& e)
    {
        spdlog::debug("Command-line parsing failed");
        return app.exit(e);
    }

    spdlog::debug("Solver selected: {}", fmt::streamed(solverType));

    // Print out the solver
    if (verbose)
    {
        fmt::println("Solver selected: {}", fmt::streamed(solverType));
    }

    // Load the puzzle(s) to a vector
    std::vector<SumokuPuzzleData> puzzles;
    if (!filePath.empty())
    {
        spdlog::debug("Loading puzzle from '{}'", filePath.string());

        if (auto puzzle = LoadPuzzle<SumokuPuzzleData>(filePath.string()); puzzle)
        {
            puzzles.push_back(*puzzle);

            spdlog::debug("Successfully loaded puzzle");
        }
        else
        {
            spdlog::error("Failed to load puzzle from '{}'", filePath.string());
            return EXIT_FAILURE;
        }
    }
    else if (!dirPath.empty())
    {
        spdlog::debug("Loading puzzles from directory '{}'", dirPath.string());
        puzzles = LoadAllPuzzles<SumokuPuzzleData>(dirPath.string());

        spdlog::debug("Loaded {} puzzle(s)", puzzles.size());
    }

    if (puzzles.empty())
    {
        spdlog::error("No valid puzzle files were loaded.");
        fmt::println(stderr, "No valid puzzle files were loaded.");
        return EXIT_FAILURE;
    }

    spdlog::info("Loaded {} puzzle(s)", puzzles.size());

    // Loop through all puzzles and solve them
    for (const auto& p : puzzles)
    {
        spdlog::debug("Solving puzzle with N = {}", p.N);
        switch (solverType)
        {
        case SolverType::SumokuSolver:
        {
            spdlog::debug("Using SumokuSolver");
            sumoku::SumokuBacktrackingSolver s {p.N, p.boxes, p.sums};
            RunSolver(s, p, benchmark);
            break;
        }
        case SolverType::SumokuMRV:
        {
            spdlog::debug("Using SumokuMRV");
            sumoku::SumokuMRVSolver s {p.N, p.boxes, p.sums};
            RunSolver(s, p, benchmark);
            break;
        }
        case SolverType::SumokuOrdering:
        {
            spdlog::debug("Using SumokuOrdering");
            sumoku::SumokuOrderingSolver s {p.N, p.boxes, p.sums};
            RunSolver(s, p, benchmark);
            break;
        }
        }
    }

    spdlog::info("Application finished successfully");

    return EXIT_SUCCESS;
}

#include <CLI/CLI.hpp>     // CLI::App, CLI::ParseError
#include <array>           // std::array
#include <chrono>          // std::chrono::steady_clock
#include <concepts>        // concept
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

#include "board/boardlib.hpp"                             // PrintBoard
#include "loader/loaderlib.hpp"                           // LoadAllPuzzles, LoadPuzzle
#include "solvers/sumoku/sumokubacktrackingsolver.hpp"    // sumoku::SumokuBacktracking
#include "solvers/sumoku/sumokubitmaskorderingsolver.hpp" // sumoku::SumokuBacktracking
#include "solvers/sumoku/sumokuorderingsolver.hpp"        // sumoku::SumokuBacktracking
#include "version.h"                                      // build_info::project_name, build_info::version, etc.

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

/// @brief Solves a puzzle and times it.
/// @tparam Solver The solver type.
/// @param solver The solver instance.
/// @return The elapsed duration.
template <typename T>
std::chrono::duration<double, std::milli> RunSolver(T& solver)
{
    auto start = std::chrono::steady_clock::now();
    solver.Solve();
    auto end = std::chrono::steady_clock::now();

    return (end - start);
}

/// @brief Prints the solution for a puzzle.
/// @tparam T The solver type.
/// @param solver The solver instance.
/// @param puzzle The puzzle data.
/// @return True if a solution was found, otherwise false.
template <typename T>
bool PrintSolution(T& solver, const SumokuPuzzleData& puzzle)
{
    if (auto board = solver.GetSolution())
    {
        fmt::println("*** Result of Puzzle #{} ***", puzzle.label);
        fmt::println("");
        PrintBoard(*board);
        fmt::println("");
        return true;
    }

    fmt::println("Failed to solve!");
    return false;
}

/// @brief Solves a puzzle and optionally prints the elapsed time.
/// @tparam T The solver type.
/// @param solver The solver instance.
/// @param puzzle The puzzle data.
/// @param benchmark Whether to print the solve duration.
/// @return True if the puzzle was solved, otherwise false.
template <typename T>
bool SolvePuzzle(T& solver, const SumokuPuzzleData& puzzle, bool benchmark)
{
    const std::chrono::duration<double, std::milli> elapsed = RunSolver(solver);

    if (!PrintSolution(solver, puzzle))
    {
        return false;
    }

    if (benchmark)
    {
        fmt::println("Solved in: {:.3f} ms", elapsed.count());
        fmt::println("");
    }

    return true;
}

/// @brief Defines the types supported as puzzle solvers.
/// @tparam T The type to test as a puzzle solver.
/// @concept PuzzleSolver
/// @details A type satisfies this concept if it supports both Solve and GetSolution functions.
template <typename T>
concept PuzzleSolver = requires(T solver) {
    solver.Solve();
    solver.GetSolution();
};

/// @brief Defines the types supported as Sumoku solvers.
/// @tparam T The type to test as a Sumoku solver.
/// @concept SumokuSolver
/// @details A type satisfies this concept if it is one of the supported Sumoku solver implementations.
template <typename T>
concept SumokuSolver =
    std::same_as<T, sumoku::SumokuBacktrackingSolver> || std::same_as<T, sumoku::SumokuMRVSolver> || std::same_as<T, sumoku::SumokuOrderingSolver>;

/// @brief Constructs a solver and solves a puzzle.
/// @tparam T The solver type.
/// @param puzzle The puzzle data.
/// @param benchmark Whether to print the solve duration.
/// @return True if the puzzle was solved, otherwise false.
template <SumokuSolver T>
requires SumokuSolver<T>
bool SolveWith(const SumokuPuzzleData& p, bool benchmark)
{
    T solver {p.N, p.boxes, p.sums};
    return SolvePuzzle(solver, p, benchmark);
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
    app.name(build_info::project_name);

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

    // Metadata
    std::string versionInfo = fmt::format("{}: {}\nBuild #: {}\nCommit: {}\nBuild date: {}", app.get_name(), build_info::version,
                                          build_info::build_number, build_info::commit_hash, build_info::build_date);
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

            std::error_code ec;
            const auto size = fs::file_size(filePath, ec);

            if (ec)
            {
                fmt::println(stderr, "Unable to determine file size: {}", ec.message());
            }

            spdlog::debug("File size: {} bytes.", size);

            if (verbose && !ec)
            {
                fmt::println("Loading puzzle from: '{}'.", filePath);
                fmt::println("File size: {} bytes.", size);
            }
        }
        else
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
        const auto solved = [&] {
            switch (solverType)
            {
            case SolverType::SumokuSolver:
            {
                spdlog::debug("Using SumokuSolver");
                return SolveWith<sumoku::SumokuBacktrackingSolver>(p, benchmark);
            }
            case SolverType::SumokuMRV:
            {
                spdlog::debug("Using SumokuMRV");
                return SolveWith<sumoku::SumokuMRVSolver>(p, benchmark);
            }
            case SolverType::SumokuOrdering:
            {
                spdlog::debug("Using SumokuOrdering");
                return SolveWith<sumoku::SumokuOrderingSolver>(p, benchmark);
            }
            }
        }(); // "()" invokes lambda

        if (!solved)
        {
            spdlog::info("Solver could not solve the puzzle.");
            return EXIT_FAILURE;
        }
    }

    spdlog::info("Application finished successfully.");

    return EXIT_SUCCESS;
}

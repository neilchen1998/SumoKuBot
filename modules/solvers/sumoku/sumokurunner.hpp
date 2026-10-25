#ifndef MODULES_SOLVERS_SUMOKU_SUMOKURUNNER_HPP
#define MODULES_SOLVERS_SUMOKU_SUMOKURUNNER_HPP

#include <chrono>          // std::chrono::steady_clock
#include <concepts>        // concept
#include <fmt/core.h>      // fmt::print
#include <spdlog/spdlog.h> // spdlog::set_level, spdlog::info

#include "board/boardlib.hpp"                          // PrintBoard
#include "loader/loaderlib.hpp"                        // LoadAllPuzzles, LoadPuzzle
#include "solvers/sumoku/sumokubacktrackingsolver.hpp" // sumoku::SumokuBacktrackingSolver
#include "solvers/sumoku/sumokumrvsolver.hpp"          // sumoku::SumokuMRVSolver
#include "solvers/sumoku/sumokuorderingsolver.hpp"     // sumoku::SumokuOrderingSolver

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

    spdlog::debug("Solved in: {:.3f} ms\n", elapsed.count());

    if (benchmark)
    {
        fmt::println("Solved in: {:.3f} ms", elapsed.count());
    }

    return true;
}

/// @brief Defines the types supported as puzzle solvers.
/// @tparam T The type to test as a puzzle solver.
/// @concept PuzzleSolver
/// @details A type satisfies this concept if it supports both Solve and GetSolution functions.
/// GetSolution must return the same type as std::optional<SudokuBoard>
template <typename T>
concept PuzzleSolver = requires(T solver) {
    { solver.Solve() };
    { solver.GetSolution() } -> std::same_as<std::optional<SudokuBoard>>;
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
bool SolveWith(const SumokuPuzzleData& p, bool benchmark)
{
    T solver {p.N, p.boxes, p.sums};
    return SolvePuzzle(solver, p, benchmark);
}

#endif // MODULES_SOLVERS_SUMOKU_SUMOKURUNNER_HPP

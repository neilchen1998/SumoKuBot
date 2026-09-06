#ifndef MODULES_SOLVERS_SUMOKU_SUMOKUMRVSOLVER_HPP
#define MODULES_SOLVERS_SUMOKU_SUMOKUMRVSOLVER_HPP

#include <cstddef>         // size_t
#include <cstdint>         // uint16_t
#include <mdspan>          // std::mdspan
#include <optional>        // std::optional
#include <spdlog/spdlog.h> // spdlog::debug, spdlog::trace
#include <vector>          // std::vector

#include "board/boardlib.hpp"    // Point, SudokuBoard
#include "solvers/selection.hpp" // Selction

namespace sumoku
{
class SumokuMRVSolver
{
  public:
    /// @brief Constructs a Sumoku solver using the minimum remaining values (MRV) heuristic.
    /// @param N The size of the board (N x N).
    /// @param boxes The groups of cells that make up each Sumoku box.
    /// @param sums The target sum for each corresponding box.
    SumokuMRVSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums);

    ~SumokuMRVSolver() = default;

    SumokuMRVSolver(const SumokuMRVSolver&) = delete;
    SumokuMRVSolver& operator=(const SumokuMRVSolver&) = delete;
    SumokuMRVSolver(SumokuMRVSolver&&) = delete;
    SumokuMRVSolver& operator=(SumokuMRVSolver&&) = delete;

    /// @brief Solves the board.
    void Solve();

    /// @brief Returns the solved Sudoku board if a solution was found.
    /// @return The solved Sudoku board, or std::nullopt if the puzzle has no solution.
    [[nodiscard]] std::optional<SudokuBoard> GetSolution() const;

  private:
    /// @brief Finds the next best cell that has the least candidates (most constrainted cell).
    /// @return The next best cell.
    Selection FindNextBestCell();

    /// @brief Solves the given Sumoku using backtracking technique.
    /// @param depth The current recursion depth of the search.
    /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack.
    bool Backtrack(size_t depth);

    /// @brief Places a number on the board in a given cell.
    /// @param r The row of the given cell.
    /// @param c The column of the given cell.
    /// @param digit The given number.
    void Place(size_t r, size_t c, size_t digit);

    /// @brief Undoes a number on the board in a given cell (the exact opposite of what Place func does).
    /// @param r The row of the given cell.
    /// @param c The column of the given cell.
    /// @param digit The given number.
    void Undo(size_t r, size_t c, size_t digit);

  private:
    /// @brief The size of the board (N x N).
    size_t N_;

    /// @brief TRUE if the solver has found a valid solution.
    bool solved_ = false;

    /// @brief Flat storage for the Sudoku board.
    std::vector<size_t> board_;

    /// @brief 2D view over the flat board storage.
    std::mdspan<size_t, std::dextents<size_t, 2>> boardView_;

    /// @brief Bitmask of digits already used in each row.
    std::vector<uint16_t> rowMask_;

    /// @brief Bitmask of digits already used in each column.
    std::vector<uint16_t> colMask_;

    /// @brief Bitmask of digits already used in each box.
    std::vector<uint16_t> boxMask_;

    /// @brief Maps each board cell to its corresponding box ID.
    SudokuBoard boxID_;

    /// @brief Remaining target sum for each box.
    std::vector<size_t> boxRemainingSum_;

    /// @brief Number of unfilled cells remaining in each box.
    std::vector<size_t> boxRemainingCells_;
};

} // namespace sumoku

#endif // MODULES_SOLVERS_SUMOKU_SUMOKUMRVSOLVER_HPP

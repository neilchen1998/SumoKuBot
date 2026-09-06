#ifndef MODULES_SOLVERS_SUDOKU_KILLERSUDOKUMRVSOLVER_HPP
#define MODULES_SOLVERS_SUDOKU_KILLERSUDOKUMRVSOLVER_HPP

#include <cstddef>         // size_t
#include <cstdint>         // uint16_t
#include <mdspan>          // std::mdspan
#include <optional>        // std::optional
#include <spdlog/spdlog.h> // spdlog::debug, spdlog::trace
#include <vector>          // std::vector

#include "board/boardlib.hpp"    // Point, SudokuBoard
#include "solvers/selection.hpp" // Selction

namespace killer_sudoku
{
class KillerSudokuMRVSolver
{
  public:
    KillerSudokuMRVSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums);

    void Solve();

    [[nodiscard]] std::optional<SudokuBoard> GetSolution() const;

  private:
    /// @brief Finds the next best cell that has the least candidates (most constrainted cell)
    /// @return The next best cell
    Selection FindNextBestCell();

    /// @brief Solves the given Sumoku using backtracking technique
    /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
    bool Backtrack();

    /// @brief Places a number on the board in a given cell
    /// @param r The row of the given cell
    /// @param c The column of the given cell
    /// @param digit The given number
    void Place(size_t r, size_t c, size_t digit);

    /// @brief Undoes a number on the board in a given cell (the exact opposite of what Place func does)
    /// @param r The row of the given cell
    /// @param c The column of the given cell
    /// @param digit The given number
    void Undo(size_t r, size_t c, size_t digit);

  private:
    size_t N_;
    bool solved_ = false;
    std::vector<size_t> board_;
    std::mdspan<size_t, std::dextents<size_t, 2>> boardView_;
    std::vector<uint16_t> rowMask_, colMask_, boxMask_, gridMask_;
    SudokuBoard boxID_;
    std::vector<size_t> boxRemainingSum_;
    std::vector<size_t> boxRemainingCells_;
};

} // namespace killer_sudoku

#endif // MODULES_SOLVERS_SUDOKU_KILLERSUDOKUMRVSOLVER_HPP

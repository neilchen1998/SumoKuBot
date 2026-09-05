#ifndef MODULES_SOLVERS_SUDOKU_SUDOKUMRV_HPP
#define MODULES_SOLVERS_SUDOKU_SUDOKUMRV_HPP

#include <cstddef>         // size_t
#include <cstdint>         // uint16_t
#include <limits>          // std::numeric_limits<size_t>::max
#include <mdspan>          // std::mdspan
#include <memory.h>        // std::make_unique, std::unique_ptr
#include <optional>        // std::optional
#include <spdlog/spdlog.h> // spdlog::debug, spdlog::trace
#include <vector>          // std::vector

#include "board/boardlib.hpp" // Point, SudokuBoard

namespace sudoku
{
/// @brief The size of a standard sudoku
constexpr size_t SUDOKU_SZ = 9;

/// @brief Solves standard 9x9 Sudoku puzzles using minimum remaining values (MRV)
class SudokuMRV
{
  public:
    SudokuMRV(const std::vector<std::vector<char>>& board);

    SudokuMRV(const std::vector<std::vector<size_t>>& board);

    void Solve();

    [[nodiscard]] std::optional<SudokuBoard> GetSolution() const;

  private:
    /// @brief The selection
    struct Selection
    {
        size_t r = std::numeric_limits<size_t>::max();
        size_t c = std::numeric_limits<size_t>::max();

        /// @brief The candidates in the mask form
        uint16_t mask = 0U;

        /// @brief TRUE if there is no other options
        bool deadEnd = false;
    };

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

    /// @brief Undoes a number on the board in a given cell (the exact opposite of what Place
    /// func does)
    /// @param r The row of the given cell
    /// @param c The column of the given cell
    /// @param digit The given number
    void Undo(size_t r, size_t c, size_t digit);

    const size_t N_;
    bool solved_ = false;
    std::vector<size_t> board_;
    std::mdspan<size_t, std::dextents<size_t, 2>> boardView_;
    std::vector<uint16_t> rowMask_, colMask_, gridMask_;
};

} // namespace sudoku

#endif // MODULES_SOLVERS_SUDOKU_SUDOKUMRV_HPP

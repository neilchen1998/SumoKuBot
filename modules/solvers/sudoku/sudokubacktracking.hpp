#ifndef MODULES_SOLVERS_SUDOKU_SUDOKUSOLVER_HPP
#define MODULES_SOLVERS_SUDOKU_SUDOKUSOLVER_HPP

#include <cstddef>  // size_t
#include <memory.h> // std::make_unique, std::unique_ptr
#include <optional> // std::optional
#include <vector>   // std::vector

#include "board/boardlib.hpp" // Point, SudokuBoard, SUDOKU_SZ

namespace sudoku
{
class SudokuBacktracking
{
  public:
    SudokuBacktracking(SudokuBoard board);

    void Solve();

    std::optional<SudokuBoard> GetSolution() const;

  private:
    /// @brief Solves the given Sudoku using backtracking technique
    /// @param board The Sudoku board
    /// @param x The current row index
    /// @param y The current column index
    /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
    bool Backtrack(SudokuBoard& board, size_t x = 0, size_t y = 0);

    /// @brief Checks if an element is valid
    /// @param board The board
    /// @param x The row index of the element
    /// @param y The column index of the element
    /// @param digit The digit of the element
    /// @return TRUE if the element is valid
    bool Check(const SudokuBoard& board, size_t x, size_t y, size_t digit);

  private:
    SudokuBoard board_;
    bool isSolved_ = false;
};

} // namespace sudoku

#endif // MODULES_SOLVERS_SUDOKU_SUDOKUSOLVER_HPP

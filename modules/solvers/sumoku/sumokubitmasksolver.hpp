#ifndef INCLUDE_SOLVER_SOLVERLIB_H_
#define INCLUDE_SOLVER_SOLVERLIB_H_

#include <cstddef> // size_t
#include <cstdint> // uint16_t
#include <vector>  // std::vector

#include "board/boardlib.hpp" // Point, SudokuBoard
#include "sumokubacktrackingsolver.hpp"

namespace sumoku
{

class SumokuBitMaskSolver final : public SumokuBacktrackingSolver
{
  public:
    SumokuBitMaskSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums);

  private:
    /// @brief Solves the given Sudoku using backtracking technique
    /// @param x The current row index
    /// @param y The current column index
    /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
    bool Backtrack(size_t x = 0, size_t y = 0) override;

    /// @brief Checks if an element is valid
    /// @param x The row index of the element
    /// @param y The column index of the element
    /// @param digit The digit of the element
    /// @return TRUE if the element is valid
    bool Check(size_t x, size_t y, size_t digit) override;

  private:
    std::vector<uint16_t> colMasks_;
    std::vector<uint16_t> rowMasks_;
};

} // namespace sumoku

#endif // INCLUDE_SOLVER_SOLVERLIB_H_

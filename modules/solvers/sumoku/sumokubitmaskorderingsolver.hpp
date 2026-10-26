#ifndef MODULES_SOLVERS_SUMOKU_SUMOKUBITMASKORDERINGSOLVER_HPP
#define MODULES_SOLVERS_SUMOKU_SUMOKUBITMASKORDERINGSOLVER_HPP

#include <cstddef>         // size_t
#include <cstdint>         // uint16_t
#include <mdspan>          // std::mdspan
#include <optional>        // std::optional
#include <spdlog/spdlog.h> // spdlog::debug, spdlog::trace
#include <vector>          // std::vector

#include "board/boardlib.hpp"    // Point, SudokuBoard
#include "solvers/selection.hpp" // Selction
#include "sumokuorderingsolver.hpp"

namespace sumoku
{
class SumokuBitMaskOrderingSolver final : public SumokuOrderingSolver
{
  public:
    SumokuBitMaskOrderingSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums);

  private:
    /// @brief Solves the given Sudoku using backtracking technique
    /// @param x The current row index
    /// @param y The current column index
    /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
    bool Backtrack(size_t idx = 0) override;

    /// @brief Checks if an element is valid
    /// @param x The row index of the element
    /// @param y The column index of the element
    /// @param digit The digit of the element
    /// @return TRUE if the element is valid
    bool IsValid(size_t x, size_t y, size_t digit) override;

  private:
    std::vector<uint16_t> colMasks_;
    std::vector<uint16_t> rowMasks_;
};

} // namespace sumoku

#endif // MODULES_SOLVERS_SUMOKU_SUMOKUBITMASKORDERINGSOLVER_HPP

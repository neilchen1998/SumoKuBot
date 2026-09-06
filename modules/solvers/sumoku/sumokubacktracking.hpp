#ifndef MODULES_SOLVERS_SUMOKU_SUMOKUBACKTRACKING_HPP
#define MODULES_SOLVERS_SUMOKU_SUMOKUBACKTRACKING_HPP

#include <cstddef>         // size_t
#include <memory.h>        // std::make_unique, std::unique_ptr
#include <optional>        // std::optional
#include <spdlog/spdlog.h> // spdlog::debug, spdlog::trace
#include <unordered_map>   // std::unordered_map
#include <vector>          // std::vector

#include "board/boardlib.hpp" // Point, SudokuBoard
#include "math/mathlib.hpp"   // PointHasher

namespace sumoku
{
class SumokuBacktracking
{
  public:
    SumokuBacktracking(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums);

    virtual ~SumokuBacktracking() = default;

    SumokuBacktracking(const SumokuBacktracking&) = delete;
    SumokuBacktracking& operator=(const SumokuBacktracking&) = delete;
    SumokuBacktracking(SumokuBacktracking&&) = delete;
    SumokuBacktracking& operator=(SumokuBacktracking&&) = delete;

    void Solve();

    [[nodiscard]] std::optional<SudokuBoard> GetSolution() const;

    void PrintBoard() const;

  protected:
    /// @brief Solves the given Sudoku using backtracking technique
    /// @param x The current row index
    /// @param y The current column index
    /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
    virtual bool Backtrack(size_t x = 0, size_t y = 0);

    /// @brief Checks if an element is valid
    /// @param x The row index of the element
    /// @param y The column index of the element
    /// @param digit The digit of the element
    /// @return TRUE if the element is valid
    virtual bool Check(size_t x, size_t y, size_t digit);

  protected:
    std::unordered_map<Point, std::vector<Point>, PointHasher> adj_;
    SudokuBoard board_;
    size_t N_;
    bool solved_;
    std::unordered_map<Point, size_t, PointHasher> sum_;
};
} // namespace sumoku

#endif // MODULES_SOLVERS_SUMOKU_SUMOKUBACKTRACKING_HPP

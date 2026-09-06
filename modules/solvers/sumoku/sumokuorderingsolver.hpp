#ifndef MODULES_SOLVERS_SUMOKU_SUMOKUORDERINGSOLVER_HPP
#define MODULES_SOLVERS_SUMOKU_SUMOKUORDERINGSOLVER_HPP

#include <cstddef>         // size_t
#include <optional>        // std::optional
#include <spdlog/spdlog.h> // spdlog::debug, spdlog::trace
#include <unordered_map>   // std::unordered_map
#include <vector>          // std::vector

#include "board/boardlib.hpp" // Point, SudokuBoard
#include "math/mathlib.hpp"   // PointHasher

namespace sumoku
{
class SumokuOrderingSolver
{
  public:
    SumokuOrderingSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums);

    virtual ~SumokuOrderingSolver() = default;

    SumokuOrderingSolver(const SumokuOrderingSolver&) = delete;
    SumokuOrderingSolver& operator=(const SumokuOrderingSolver&) = delete;
    SumokuOrderingSolver(SumokuOrderingSolver&&) = delete;
    SumokuOrderingSolver& operator=(SumokuOrderingSolver&&) = delete;

    void Solve();

    [[nodiscard]] std::optional<SudokuBoard> GetSolution() const;

    void PrintBoard() const;

  protected:
    /// @brief Solves the given Sudoku using backtracking technique
    /// @param idx The current visit index of the element
    /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
    virtual bool Backtrack(size_t idx = 0);

    /// @brief Checks if an element is valid
    /// @param x The row index of the element
    /// @param y The column index of the element
    /// @param digit The digit of the element
    /// @return TRUE if the element is valid
    virtual bool IsValid(size_t x, size_t y, size_t digit);

  protected:
    /// @brief The Sudoku board containing the current puzzle state.
    SudokuBoard board_;

    /// @brief Collection of boxes (subgrids), with each box represented by its points.
    std::vector<std::vector<Point>> boxes_;

    /// @brief Maps each point to the points belonging to the same box.
    std::unordered_map<Point, std::vector<Point>, PointHasher> boxMembers_;

    /// @brief Dimension of the Sudoku board (e.g., 9 for a 9x9 board).
    size_t N_;

    /// @brief Tracks the current sum/value associated with each point during solving.
    std::unordered_map<Point, size_t, PointHasher> sum_;

    /// @brief Indicates whether the puzzle has been successfully solved.
    bool solved_;

    /// @brief Order in which points are visited during the solving process.
    std::vector<Point> visitOrder_;
};
} // namespace sumoku

#endif // MODULES_SOLVERS_SUMOKU_SUMOKUORDERINGSOLVER_HPP

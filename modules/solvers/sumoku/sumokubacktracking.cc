#include "sumokubacktracking.hpp"

#include <cstddef>         // size_t
#include <memory.h>        // std::make_unique, std::unique_ptr
#include <optional>        // std::optional
#include <spdlog/spdlog.h> // spdlog::debug, spdlog::trace
#include <unordered_map>   // std::unordered_map
#include <vector>          // std::vector

#include "board/boardlib.hpp" // Point, SudokuBoard

namespace sumoku
{
SumokuBacktracking::SumokuBacktracking(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
    : board_(N, std::vector<size_t>(N, 0)), N_(N), solved_(false)
{
    // Create the adjacent list
    for (auto& box : boxes)
    {
        for (size_t i = 0; i < box.size(); ++i)
        {
            for (size_t j = 0; j < box.size(); ++j)
            {
                // Only add the point if the current point is not itself
                if (i != j)
                {
                    adj_[box[i]].emplace_back(box[j]);
                }
            }
        }
    }

    // Create the sum map
    for (size_t i = 0; i < boxes.size(); ++i)
    {
        for (auto& p : boxes[i])
        {
            sum_[p] = sums[i];
        }
    }
}

void SumokuBacktracking::Solve()
{
    solved_ = Backtrack();
}

[[nodiscard]] std::optional<SudokuBoard> SumokuBacktracking::GetSolution() const
{
    return solved_ ? std::optional<SudokuBoard> {board_} : std::nullopt;
}

void SumokuBacktracking::PrintBoard() const
{
    ::PrintBoard(board_);
}

bool SumokuBacktracking::Backtrack(size_t x, size_t y)
{
    // If we reach the last column, then we start from the next row
    if (y == N_)
    {
        return Backtrack(x + 1, 0);
    }

    // If we reach to the end of the Sudoku board, then we have found a valid solution
    if (x == N_)
    {
        return true;
    }

    // If there is already a value on the current element, then we skip it
    if (board_[x][y] != 0)
    {
        return Backtrack(x, y + 1);
    }

    // We can put any number from 1 to N_
    for (size_t digit = 1; digit <= N_; ++digit)
    {
        // If the current guess is valid, then we write the current element with the guess
        if (Check(x, y, digit))
        {
            board_[x][y] = digit;

            // Trigger another backtrack
            if (Backtrack(x, y + 1))
            {
                return true;
            }

            // The current guess is incorrect, we re-write it with a default value
            // NOTE: if the guess were correct, then we would exit early and would not reach here
            board_[x][y] = 0;
        }
    }

    return false;
}

bool SumokuBacktracking::Check(size_t x, size_t y, size_t digit)
{
    // Check if there is any duplicate row-wise
    for (size_t i = 0; i < N_; ++i)
    {
        if (board_[i][y] == digit)
        {
            return false;
        }
    }

    // Check if there is any duplicate column-wise
    for (size_t j = 0; j < N_; ++j)
    {
        if (board_[x][j] == digit)
        {
            return false;
        }
    }

    // Check if the box matches the sum
    bool isFilled = true;
    size_t curSum = digit;
    for (auto& [u, v] : adj_[{x, y}])
    {
        // As long as there is an element that is zero, that means the cage is not filled yet
        if (board_[u][v] == 0)
        {
            isFilled = false;
        }
        curSum += board_[u][v];
    }

    // The cage restriction is met when:
    // 1. the cage has been filled and the current sum equals to the target sum
    // 2. the cage has not yet filled and the current sum is less than the target sum
    if ((isFilled && curSum == sum_[{x, y}]) || (!isFilled && curSum < sum_[{x, y}]))
    {
        return true;
    }

    return false;
}
} // namespace sumoku

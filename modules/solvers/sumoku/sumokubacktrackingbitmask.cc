#include "sumokubacktrackingbitmask.hpp"

#include <cstddef> // size_t
#include <vector>  // std::vector

#include "board/boardlib.hpp"     // Point, SudokuBoard
#include "sumokubacktracking.hpp" // SumokuBacktracking

namespace sumoku
{

SumokuSolverWithBitMask::SumokuSolverWithBitMask(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
    : SumokuBacktracking(N, boxes, sums), // let the base class constructor handle
      colMasks_(N, 0), rowMasks_(N, 0)
{
}

bool SumokuSolverWithBitMask::Backtrack(size_t x, size_t y)
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
        uint_fast16_t bit = (1U << digit);
        if (!(rowMasks_[x] & bit) && !(colMasks_[y] & bit))
        {
            // If the current guess is valid, then we write the current element with the guess
            if (Check(x, y, digit))
            {
                board_[x][y] = digit;
                rowMasks_[x] |= bit;
                colMasks_[y] |= bit;

                // Trigger another backtrack
                if (Backtrack(x, y + 1))
                {
                    return true;
                }

                // The current guess is incorrect, we re-write it with a default value
                // NOTE: if the guess were correct, then we would exit early and would not reach here
                board_[x][y] = 0;
                rowMasks_[x] &= ~bit;
                colMasks_[y] &= ~bit;
            }
        }
    }

    return false;
}

bool SumokuSolverWithBitMask::Check(size_t x, size_t y, size_t digit)
{
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

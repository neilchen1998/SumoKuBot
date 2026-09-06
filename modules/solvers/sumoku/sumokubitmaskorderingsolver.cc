#include "sumokubitmaskorderingsolver.hpp"

#include <cstddef>         // size_t
#include <cstdint>         // uint16_t
#include <spdlog/spdlog.h> // spdlog::debug, spdlog::trace
#include <unordered_map>   // std::unordered_map
#include <vector>          // std::vector

#include "board/boardlib.hpp" // Point, SudokuBoard

namespace sumoku
{
SumokuBitMaskOrderingSolver::SumokuBitMaskOrderingSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
    : SumokuOrderingSolver(N, boxes, sums), colMasks_(N, 0), rowMasks_(N, 0)
{
}

bool SumokuBitMaskOrderingSolver::Backtrack(size_t idx)
{
    if (idx == visitOrder_.size())
    {
        return true;
    }

    Point p = visitOrder_[idx];
    for (size_t digit = 1; digit <= N_; ++digit)
    {
        uint_fast16_t bit = (1U << digit);
        if (!(rowMasks_[p.x] & bit) && !(colMasks_[p.y] & bit))
        {
            if (IsValid(p.x, p.y, digit))
            {
                board_[p.x][p.y] = digit;
                rowMasks_[p.x] |= bit;
                colMasks_[p.y] |= bit;

                if (Backtrack(idx + 1))
                {
                    return true;
                }

                board_[p.x][p.y] = 0;
                rowMasks_[p.x] &= ~bit;
                colMasks_[p.y] &= ~bit;
            }
        }
    }

    return false;
}

bool SumokuBitMaskOrderingSolver::IsValid(size_t x, size_t y, size_t digit)
{
    // Check the box
    size_t curSum = digit;
    bool isFull = true;

    for (const auto& member : boxMembers_[{x, y}])
    {
        if (member.x == x && member.y == y)
        {
            continue;
        }

        size_t ele = board_[member.x][member.y];

        // Check if the element is zero, if so that means the current box is not full
        if (ele == 0)
        {
            isFull = false;
        }
        else if (ele == digit)
        {
            // there can only be one unique number in a given box
            return false;
        }
        else
        {
            curSum += ele;
        }
    }

    size_t target = sum_[{x, y}];

    // If the current sum exceeds the target then we prune it by returning false
    if (curSum > target)
    {
        return false;
    }

    // If the cage is full then the current sum must equal to the target
    if (isFull)
    {
        return (curSum == target);
    }

    return true;
}
} // namespace sumoku

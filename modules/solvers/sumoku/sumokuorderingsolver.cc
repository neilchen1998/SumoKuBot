#include "sumokuorderingsolver.hpp"

#include <bit>             // std::popcount
#include <cstddef>         // size_t
#include <cstdint>         // uint16_t
#include <limits>          // std::numeric_limits<size_t>::max
#include <mdspan>          // std::mdspan
#include <numeric>         // std::iota
#include <optional>        // std::optional
#include <spdlog/spdlog.h> // spdlog::debug, spdlog::trace
#include <unordered_map>   // std::unordered_map
#include <vector>          // std::vector

#include "board/boardlib.hpp" // Point, SudokuBoard
#include "math/mathlib.hpp"   // PointHasher

namespace sumoku
{
SumokuOrderingSolver::SumokuOrderingSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
    : board_(N, std::vector<size_t>(N, 0)), boxes_(boxes), N_(N), solved_(false)
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
                    boxMembers_[box[i]].emplace_back(box[j]);
                }
            }
        }
    }

    // Create a vector of indices
    std::vector<size_t> indices(boxes.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Sort the indices based on the size of the boxes
    std::sort(indices.begin(), indices.end(), [&](size_t lhs, size_t rhs) { return boxes_[lhs].size() < boxes_[rhs].size(); });

    // Construct the visit order based on the size of the boxes
    for (size_t idx : indices)
    {
        visitOrder_.insert(visitOrder_.end(), boxes_[idx].begin(), boxes_[idx].end());
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

void SumokuOrderingSolver::Solve()
{
    solved_ = Backtrack();
}

[[nodiscard]] std::optional<SudokuBoard> SumokuOrderingSolver::GetSolution() const
{
    return solved_ ? std::optional<SudokuBoard> {board_} : std::nullopt;
}

void SumokuOrderingSolver::PrintBoard() const
{
    ::PrintBoard(board_);
}

bool SumokuOrderingSolver::Backtrack(size_t idx)
{
    if (idx == visitOrder_.size())
    {
        return true;
    }

    Point p = visitOrder_[idx];
    for (size_t digit = 1; digit <= N_; ++digit)
    {
        if (IsValid(p.x, p.y, digit))
        {
            board_[p.x][p.y] = digit;

            if (Backtrack(idx + 1))
            {
                return true;
            }

            board_[p.x][p.y] = 0;
        }
    }

    return false;
}

bool SumokuOrderingSolver::IsValid(size_t x, size_t y, size_t digit)
{
    // Check the row and the column
    for (size_t i = 0; i < N_; ++i)
    {
        if ((board_[x][i] == digit) || (board_[i][y] == digit))
        {
            return false;
        }
    }

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

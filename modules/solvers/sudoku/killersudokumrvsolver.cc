#include "killersudokumrvsolver.hpp"

#include <bit>             // std::popcount
#include <cstddef>         // size_t
#include <cstdint>         // uint16_t
#include <limits>          // std::numeric_limits<size_t>::max
#include <mdspan>          // std::mdspan
#include <optional>        // std::optional
#include <spdlog/spdlog.h> // spdlog::debug, spdlog::trace
#include <vector>          // std::vector

#include "board/boardlib.hpp" // Point, SudokuBoard

namespace killer_sudoku
{
KillerSudokuMRVSolver::KillerSudokuMRVSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
    : N_(N), board_(N * N, 0), boardView_(board_.data(), N, N), rowMask_(N, 0), colMask_(N, 0), boxMask_(boxes.size(), 0),
      gridMask_((N / 3) * (N / 3), 0), boxID_(N, std::vector<size_t>(N, 0)), boxRemainingSum_(sums.begin(), sums.end()),
      boxRemainingCells_(sums.size(), 0)
{
    for (size_t i = 0; i < boxes.size(); ++i)
    {
        boxRemainingCells_[i] = boxes[i].size();

        for (const Point& p : boxes[i])
        {
            boxID_[p.x][p.y] = i;
        }
    }
}

void KillerSudokuMRVSolver::Solve()
{
    solved_ = Backtrack();
}

[[nodiscard]] std::optional<SudokuBoard> KillerSudokuMRVSolver::GetSolution() const
{
    if (!solved_)
    {
        return std::nullopt;
    }

    SudokuBoard ret(N_, std::vector<size_t>(N_));

    for (size_t r = 0; r < N_; ++r)
    {
        for (size_t c = 0; c < N_; ++c)
        {
            ret[r][c] = boardView_[r, c];
        }
    }

    return ret;
}

Selection KillerSudokuMRVSolver::FindNextBestCell()
{
    Selection ret;
    int curMinCnt = sudoku::SUDOKU_SZ + 1;

    // Loop through the entire board to find the next best cell
    for (size_t r = 0; r < N_; ++r)
    {
        for (size_t c = 0; c < N_; ++c)
        {
            size_t id = boxID_[r][c];
            size_t gridID = ((r / 3) * 3) + (c / 3);

            // Only check the cell that is empty
            if (boardView_[r, c] == 0)
            {
                // Get the candidates and the number of candidates
                uint16_t sumMask = GetPossibleNumbersMask(boxRemainingSum_[id], boxRemainingCells_[id]);
                uint16_t candidates = ~(rowMask_[r] | colMask_[c] | boxMask_[id] | gridMask_[gridID]) & sumMask;

                // If there is no candidate available that means we hit a dead end and this tree needs to be pruned
                if (candidates == 0) [[unlikely]]
                {
                    return Selection {.deadEnd = true};
                }

                int curNumOfCandidates = std::popcount(candidates);

                // Update the return value when the current number of candidates is smaller than the previous one
                if (curNumOfCandidates < curMinCnt)
                {
                    curMinCnt = curNumOfCandidates;
                    ret.r = r;
                    ret.c = c;
                    ret.mask = candidates;

                    // If there is only one candidate then we return the current value early
                    if (curNumOfCandidates == 1)
                    {
                        return ret;
                    }
                }
            }
        }
    }

    return ret;
}

bool KillerSudokuMRVSolver::Backtrack()
{
    Selection next = FindNextBestCell();

    // If the next best cell is illegal, that means backtracking fails
    if (next.deadEnd)
    {
        return false;
    }

    // If there is no next best cell and we are not hitting a dead end that means we have finished the entire board
    if (next.r == std::numeric_limits<size_t>::max() && next.c == std::numeric_limits<size_t>::max())
    {
        return true;
    }

    // Loop from number 1 to N
    for (size_t digit = 1; digit <= N_; ++digit)
    {
        if (next.mask & (1U << digit))
        {
            Place(next.r, next.c, digit);
            if (Backtrack())
            {
                return true;
            }
            Undo(next.r, next.c, digit);
        }
    }

    return false;
}

void KillerSudokuMRVSolver::Place(size_t r, size_t c, size_t digit)
{
    size_t id = boxID_[r][c];
    size_t gridID = ((r / 3) * 3) + (c / 3);

    boardView_[r, c] = digit;
    rowMask_[r] |= (1U << digit);
    colMask_[c] |= (1U << digit);
    boxMask_[id] |= (1U << digit);
    gridMask_[gridID] |= (1U << digit);
    boxRemainingSum_[id] -= digit;
    --boxRemainingCells_[id];
}

void KillerSudokuMRVSolver::Undo(size_t r, size_t c, size_t digit)
{
    size_t id = boxID_[r][c];
    size_t gridID = ((r / 3) * 3) + (c / 3);

    boardView_[r, c] = 0;
    rowMask_[r] &= ~(1U << digit);
    colMask_[c] &= ~(1U << digit);
    boxMask_[id] &= ~(1U << digit);
    gridMask_[gridID] &= ~(1U << digit);
    boxRemainingSum_[id] += digit;
    ++boxRemainingCells_[id];
}
} // namespace killer_sudoku

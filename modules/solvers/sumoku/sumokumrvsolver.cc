#include "sumokumrvsolver.hpp"

#include <cstddef>         // size_t
#include <cstdint>         // uint16_t
#include <mdspan>          // std::mdspan
#include <optional>        // std::optional
#include <spdlog/spdlog.h> // spdlog::debug, spdlog::trace
#include <vector>          // std::vector

#include "board/boardlib.hpp"    // Point, SudokuBoard
#include "solvers/selection.hpp" // Selction

namespace sumoku
{
SumokuMRVSolver::SumokuMRVSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
    : N_(N), board_(N * N, 0), boardView_(board_.data(), N, N), rowMask_(N, 0), colMask_(N, 0), boxMask_(boxes.size(), 0),
      boxID_(N, std::vector<size_t>(N, 0)), boxRemainingSum_(sums.begin(), sums.end()), boxRemainingCells_(sums.size(), 0)
{
    // spdlog::info("Starting Sumoku solver");
    for (size_t i = 0; i < boxes.size(); ++i)
    {
        boxRemainingCells_[i] = boxes[i].size();

        for (const Point& p : boxes[i])
        {
            boxID_[p.x][p.y] = i;
        }
    }
}

void SumokuMRVSolver::Solve()
{
    solved_ = Backtrack(0);
    // spdlog::info("SumokuMRV::Solve() finished, solved={}", solved_);
}

[[nodiscard]] std::optional<SudokuBoard> SumokuMRVSolver::GetSolution() const
{
    if (!solved_)
    {
        spdlog::debug("GetSolution() called but solver has no solution");
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

Selection SumokuMRVSolver::FindNextBestCell()
{
    Selection ret;
    int curMinCnt = std::numeric_limits<int>::max();

    // Loop through the entire board to find the next best cell
    for (size_t r = 0; r < N_; ++r)
    {
        for (size_t c = 0; c < N_; ++c)
        {
            size_t id = boxID_[r][c];

            // Only check the cell that is empty
            if (boardView_[r, c] == 0)
            {
                // Get the candidates and the number of candidates
                uint16_t sumMask = GetPossibleNumbersMask(boxRemainingSum_[id], boxRemainingCells_[id]);
                uint16_t candidates = ~(rowMask_[r] | colMask_[c] | boxMask_[id]) & sumMask;

#ifdef __GNUC__
                int curNumOfCandidates = __builtin_popcount(candidates);
#else
                int curNumOfCandidates = std::popcount(candidates);
#endif

                spdlog::trace("MRV check: cell=({}, {}), box={}, remainingSum={}, "
                              "remainingCells={}, sumMask=0x{:04x}, candidates=0x{:04x}, "
                              "candidateCount={}",
                              r, c, id, boxRemainingSum_[id], boxRemainingCells_[id], sumMask, candidates, curNumOfCandidates);

                // Early return if there is only a single candidate based on the box
                if (curNumOfCandidates == 1)
                {
                    spdlog::debug("MRV selected cell=({}, {}) due to single sum candidate, "
                                  "mask=0x{:04x}",
                                  r, c, candidates);

                    return {.r = r, .c = c, .mask = candidates};
                }

                // If there is no candidate available that means we hit a dead end and this tree needs to be pruned
                if (candidates == 0) [[unlikely]]
                {
                    spdlog::debug("MRV dead end at cell=({}, {}), box={}, "
                                  "remainingSum={}, remainingCells={}",
                                  r, c, id, boxRemainingSum_[id], boxRemainingCells_[id]);

                    return Selection {.deadEnd = true};
                }

                // Update the return value when the current number of candidates is smaller than the previous one
                if (curNumOfCandidates < curMinCnt)
                {
                    curMinCnt = curNumOfCandidates;
                    ret.r = r;
                    ret.c = c;
                    ret.mask = candidates;

                    spdlog::debug("MRV new best: cell=({}, {}), box={}, candidates=0x{:04x}, "
                                  "count={}",
                                  r, c, id, candidates, curNumOfCandidates);

                    // If there is only one candidate then we return the current value early
                    if (curNumOfCandidates == 1)
                    {
                        return ret;
                    }
                }
            }
        }
    }

    if (ret.r == std::numeric_limits<size_t>::max())
    {
        spdlog::debug("MRV found no empty cells");
    }

    return ret;
}

bool SumokuMRVSolver::Backtrack(size_t depth)
{
    Selection next = FindNextBestCell();

    // If the next best cell is illegal, that means backtracking fails
    if (next.deadEnd)
    {
        spdlog::debug("Backtrack: dead end at depth={}, returning false", depth);

        return false;
    }

    // If there is no next best cell and we are not hitting a dead end that means we have finished the entire board
    if (next.r == std::numeric_limits<size_t>::max() && next.c == std::numeric_limits<size_t>::max())
    {
        spdlog::debug("Backtrack: solution found at depth={}", depth);
        return true;
    }

    spdlog::debug("Backtrack: depth={}, selected cell=({}, {}), mask=0x{:04x}", depth, next.r, next.c, next.mask);

    // Loop from number 1 to N
    for (size_t digit = 1; digit <= N_; ++digit)
    {
        if (next.mask & (1U << digit))
        {
            spdlog::trace("Backtrack: depth={}, trying digit {} at cell=({}, {})", depth, digit, next.r, next.c);

            Place(next.r, next.c, digit);
            if (Backtrack(depth + 1))
            {
                spdlog::debug("Backtrack: depth={}, digit {} at cell=({}, {}) succeeded", depth, digit, next.r, next.c);

                return true;
            }

            spdlog::trace("Backtrack: depth={}, digit {} at cell=({}, {}) failed, undoing", depth, digit, next.r, next.c);

            Undo(next.r, next.c, digit);
        }
    }

    spdlog::debug("Backtrack: all candidates exhausted at depth={}", depth);

    return false;
}

void SumokuMRVSolver::Place(size_t r, size_t c, size_t digit)
{
    size_t id = boxID_[r][c];

    boardView_[r, c] = digit;
    rowMask_[r] |= (1U << digit);
    colMask_[c] |= (1U << digit);
    boxMask_[id] |= (1U << digit);
    boxRemainingSum_[id] -= digit;
    --boxRemainingCells_[id];
}

void SumokuMRVSolver::Undo(size_t r, size_t c, size_t digit)
{
    size_t id = boxID_[r][c];

    boardView_[r, c] = 0;
    rowMask_[r] &= ~(1U << digit);
    colMask_[c] &= ~(1U << digit);
    boxMask_[id] &= ~(1U << digit);
    boxRemainingSum_[id] += digit;
    ++boxRemainingCells_[id];
}

} // namespace sumoku

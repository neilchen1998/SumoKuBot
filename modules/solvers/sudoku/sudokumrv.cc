#include "sudokumrv.hpp"

#include <bit>             // std::popcount
#include <cstddef>         // size_t
#include <cstdint>         // uint16_t
#include <limits>          // std::numeric_limits<size_t>::max
#include <mdspan>          // std::mdspan
#include <memory.h>        // std::make_unique, std::unique_ptr
#include <optional>        // std::optional
#include <spdlog/spdlog.h> // spdlog::debug, spdlog::trace
#include <vector>          // std::vector

namespace sudoku
{
SudokuMRV::SudokuMRV(const std::vector<std::vector<char>>& board)
    : N_(SUDOKU_SZ), board_(N_ * N_, 0), boardView_(board_.data(), N_, N_), rowMask_(N_, 0), colMask_(N_, 0), gridMask_(N_, 0)
{
    for (size_t r = 0; r < N_; ++r)
    {
        for (size_t c = 0; c < N_; ++c)
        {
            char val = board[r][c];
            if (val >= '1' && val <= '9')
            {
                size_t digit = val - '0';
                Place(r, c, digit);
            }
        }
    }
}

SudokuMRV::SudokuMRV(const std::vector<std::vector<size_t>>& board)
    : N_(SUDOKU_SZ), board_(N_ * N_, 0), boardView_(board_.data(), N_, N_), rowMask_(N_, 0), colMask_(N_, 0), gridMask_(N_, 0)
{
    for (size_t r = 0; r < N_; ++r)
    {
        for (size_t c = 0; c < N_; ++c)
        {
            auto val = board[r][c];
            if ((val >= 1) && (val <= 9))
            {
                Place(r, c, board[r][c]);
            }
        }
    }
}

void SudokuMRV::Solve()
{
    solved_ = Backtrack();
}

[[nodiscard]] std::optional<SudokuBoard> SudokuMRV::GetSolution() const
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

SudokuMRV::Selection SudokuMRV::FindNextBestCell()
{
    Selection ret;
    int curMinCnt = std::numeric_limits<int>::max();

    for (size_t r = 0; r < 9; ++r)
    {
        for (size_t c = 0; c < 9; ++c)
        {
            if (boardView_[r, c] == 0)
            {
                size_t gridID = (r / 3) * 3 + (c / 3);

                // Get all the candidates and the number of candidates
                uint16_t candidates = ((1U << (N_ + 1)) - 2); // 1111111110U
                candidates &= ~(rowMask_[r] | colMask_[c] | gridMask_[gridID]);

                // If there is no candidate available that means we hit a dead end and this tree needs to be pruned
                if (candidates == 0U) [[unlikely]]
                {
                    return Selection {.deadEnd = true};
                }

                // Update the return value when the current number of candidates is smaller than the previous one
                if (int curNumOfCandidates = std::popcount(candidates); curNumOfCandidates < curMinCnt)
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

/// @brief Solves the given Sumoku using backtracking technique
/// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
bool SudokuMRV::Backtrack()
{
    Selection next = FindNextBestCell();

    // If the next best cell is illegal, that means backtracking fails
    if (next.deadEnd)
    {
        return false;
    }

    // If there is no next best cell and we are not hitting a dead end that means we have finished the entire board
    if (next.r == std::numeric_limits<size_t>::max())
    {
        return true;
    }

    // Loop from number 1 to 9 (inclusive)
    for (size_t digit = 1; digit <= 9; ++digit)
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

/// @brief Places a number on the board in a given cell
/// @param r The row of the given cell
/// @param c The column of the given cell
/// @param digit The given number
void SudokuMRV::Place(size_t r, size_t c, size_t digit)
{
    size_t gridID = (r / 3) * 3 + (c / 3);
    boardView_[r, c] = digit;
    rowMask_[r] |= (1U << digit);
    colMask_[c] |= (1U << digit);
    gridMask_[gridID] |= (1U << digit);
}

/// @brief Undoes a number on the board in a given cell (the exact opposite of what Place
/// func does)
/// @param r The row of the given cell
/// @param c The column of the given cell
/// @param digit The given number
void SudokuMRV::Undo(size_t r, size_t c, size_t digit)
{
    size_t gridID = (r / 3) * 3 + (c / 3);
    boardView_[r, c] = 0;
    rowMask_[r] &= ~(1U << digit);
    colMask_[c] &= ~(1U << digit);
    gridMask_[gridID] &= ~(1U << digit);
}
} // namespace sudoku

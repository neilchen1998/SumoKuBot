#ifndef INCLUDE_SOLVER_SOLVERLIB_H_
#define INCLUDE_SOLVER_SOLVERLIB_H_

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

#include "board/boardlib.hpp"   // Point, SudokuBoard
#include "math/mathlib.hpp"   // PointHasher

namespace solver
{
    class SumokuOrderingSolver
    {
    public:
        SumokuOrderingSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
        : board_(N, std::vector<size_t>(N, 0)),
        boxes_(boxes),
        N_(N),
        solved_(false)
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
            std::sort(indices.begin(), indices.end(), [&](size_t lhs, size_t rhs)
            {
                return boxes_[lhs].size() < boxes_[rhs].size();
            });

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

        void Solve()
        {
            solved_ = Backtrack();
        }

        [[nodiscard]] std::optional<SudokuBoard> GetSolution() const
        {
            return solved_ ? std::optional<SudokuBoard>{board_} : std::nullopt;
        }

        void PrintBoard() const
        {
            ::PrintBoard(board_);
        }

    protected:
        /// @brief Solves the given Sudoku using backtracking technique
        /// @param idx The current visit index of the element
        /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
        virtual bool Backtrack(size_t idx = 0)
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

        /// @brief Checks if an element is valid
        /// @param x The row index of the element
        /// @param y The column index of the element
        /// @param digit The digit of the element
        /// @return TRUE if the element is valid
        virtual bool IsValid(size_t x, size_t y, size_t digit)
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

    protected:
        SudokuBoard board_;
        std::vector<std::vector<Point>> boxes_;
        std::unordered_map<Point, std::vector<Point>, PointHasher> boxMembers_;
        size_t N_;
        std::unordered_map<Point, size_t, PointHasher> sum_;
        bool solved_;
        std::vector<Point> visitOrder_;
    };

    class SumokBitMaskSolver final : public SumokuOrderingSolver
    {
    public:
        SumokBitMaskSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
        : SumokuOrderingSolver(N, boxes, sums),
        colMasks_(N, 0),
        rowMasks_(N, 0)
        {
        }

    private:
        /// @brief Solves the given Sudoku using backtracking technique
        /// @param x The current row index
        /// @param y The current column index
        /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
        bool Backtrack(size_t idx = 0) override
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

        /// @brief Checks if an element is valid
        /// @param x The row index of the element
        /// @param y The column index of the element
        /// @param digit The digit of the element
        /// @return TRUE if the element is valid
        bool IsValid(size_t x, size_t y, size_t digit) override
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

    private:
        std::vector<uint16_t> colMasks_;
        std::vector<uint16_t> rowMasks_;
    };

    class SumokuMRVSolver
    {
      public:
        /// @brief Constructs a Sumoku solver using the minimum remaining values (MRV) heuristic.
        /// @param N The size of the board (N x N).
        /// @param boxes The groups of cells that make up each Sumoku box.
        /// @param sums The target sum for each corresponding box.
        SumokuMRVSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
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

        /// @brief Solves the board.
        void Solve()
        {
            solved_ = Backtrack(0);
            // spdlog::info("SumokuMRV::Solve() finished, solved={}", solved_);
        }

        /// @brief Returns the solved Sudoku board if a solution was found.
        /// @return The solved Sudoku board, or std::nullopt if the puzzle has no solution.
        [[nodiscard]] std::optional<SudokuBoard> GetSolution() const
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

      private:
        /// @brief The selection
        struct Selection
        {
            size_t r = std::numeric_limits<size_t>::max();
            size_t c = std::numeric_limits<size_t>::max();

            /// @brief The candidates in the mask form
            uint16_t mask = 0U;

            /// @brief TRUE if there is no other options
            bool deadEnd = false;
        };

        /// @brief Finds the next best cell that has the least candidates (most constrainted cell).
        /// @return The next best cell.
        Selection FindNextBestCell()
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

        /// @brief Solves the given Sumoku using backtracking technique.
        /// @param depth The current recursion depth of the search.
        /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack.
        bool Backtrack(size_t depth)
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

        /// @brief Places a number on the board in a given cell.
        /// @param r The row of the given cell.
        /// @param c The column of the given cell.
        /// @param digit The given number.
        void Place(size_t r, size_t c, size_t digit)
        {
            size_t id = boxID_[r][c];

            boardView_[r, c] = digit;
            rowMask_[r] |= (1U << digit);
            colMask_[c] |= (1U << digit);
            boxMask_[id] |= (1U << digit);
            boxRemainingSum_[id] -= digit;
            --boxRemainingCells_[id];
        }

        /// @brief Undoes a number on the board in a given cell (the exact opposite of what Place func does).
        /// @param r The row of the given cell.
        /// @param c The column of the given cell.
        /// @param digit The given number.
        void Undo(size_t r, size_t c, size_t digit)
        {
            size_t id = boxID_[r][c];

            boardView_[r, c] = 0;
            rowMask_[r] &= ~(1U << digit);
            colMask_[c] &= ~(1U << digit);
            boxMask_[id] &= ~(1U << digit);
            boxRemainingSum_[id] += digit;
            ++boxRemainingCells_[id];
        }

      private:
        /// @brief The size of the board (N x N).
        size_t N_;

        /// @brief TRUE if the solver has found a valid solution.
        bool solved_ = false;

        /// @brief Flat storage for the Sudoku board.
        std::vector<size_t> board_;

        /// @brief 2D view over the flat board storage.
        std::mdspan<size_t, std::dextents<size_t, 2>> boardView_;

        /// @brief Bitmask of digits already used in each row.
        std::vector<uint16_t> rowMask_;

        /// @brief Bitmask of digits already used in each column.
        std::vector<uint16_t> colMask_;

        /// @brief Bitmask of digits already used in each box.
        std::vector<uint16_t> boxMask_;

        /// @brief Maps each board cell to its corresponding box ID.
        SudokuBoard boxID_;

        /// @brief Remaining target sum for each box.
        std::vector<size_t> boxRemainingSum_;

        /// @brief Number of unfilled cells remaining in each box.
        std::vector<size_t> boxRemainingCells_;
    };
}   // solver

#endif // INCLUDE_SOLVER_SOLVERLIB_H_

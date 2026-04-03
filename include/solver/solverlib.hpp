#ifndef INCLUDE_SOLVER_SOLVERLIB_H_
#define INCLUDE_SOLVER_SOLVERLIB_H_

#include <algorithm>    // std::algorithm
#include <cstdint>  // uint16_t
#include <limits>   // std::numeric_limits<size_t>::max
#include <mdspan>   // std::mdspan
#include <numeric>   // std::iota
#include <optional> // std::optional
#include <unordered_map>   // std::unordered_map
#include <vector>   // std::vector

#ifndef __GNUC__
#include <bit>  // std::popcount
#endif

#include <fmt/core.h>   // fmt::print

#include "board/boardlib.hpp"   // Point, SudokuBoard
#include "math/mathlib.hpp"   // PointHasher

using SudokuBoard = std::vector<std::vector<size_t>>;

namespace solver
{
    class SudokuSolver
    {
    public:
        SudokuSolver(std::vector<std::vector<char>>& board)
        : board_(board),
        isSolved_(false)
        {

        }

        void Solve()
        {
            isSolved_ = Backtrack(board_);
        }

        std::optional<std::vector<std::vector<char>>> GetSolution() const
        {
            return isSolved_ ? std::optional<std::vector<std::vector<char>>>{board_} : std::nullopt;
        }

    private:
        /// @brief Solves the given Sudoku using backtracking technique
        /// @param board The Sudoku board
        /// @param x The current row index
        /// @param y The current column index
        /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
        bool Backtrack(std::vector<std::vector<char>>& board, size_t x = 0, size_t y = 0)
        {
            // If we reach the last column, then we start from the next row
            if (y == 9)
            {
                return Backtrack(board, x + 1, 0);
            }

            // If we reach to the end of the Sudoku board, then we have found a valid solution
            if (x == 9)
            {
                return true;
            }

            // If there is already a value on the current element, then we skip it
            if (board[x][y] != '.')
            {
                return Backtrack(board, x, y + 1);
            }

            // We can put any number from 1 to 9
            for (char c = '1'; c <= '9'; ++c)
            {
                // If the current guess is valid, then we write the current element with the guess
                if (Check(board, x, y, c))
                {
                    board[x][y] = c;

                    // Trigger another backtrack
                    if (Backtrack(board, x, y + 1))
                    {
                        return true;
                    }

                    // The current guess is incorrect, we re-write it with a default value
                    // NOTE: if the guess were correct, then we would exit early and would not reach here
                    board[x][y] = '.';
                }
            }

            return false;
        }

        /// @brief Checks if an element is valid
        /// @param board The board
        /// @param x The row index of the element
        /// @param y The column index of the element
        /// @param digit The digit of the element
        /// @return TRUE if the element is valid
        bool Check(const std::vector<std::vector<char>>& board, size_t x, size_t y, char digit)
        {
            // Check if there is any duplicate row-wise
            for (size_t i = 0; i < 9; ++i)
            {
                if (board[i][y] == digit)
                {
                    return false;
                }
            }

            // Check if there is any duplicate column-wise
            for (size_t j = 0; j < 9; ++j)
            {
                if (board[x][j] == digit)
                {
                    return false;
                }
            }

            // Check if there is any duplicate grid-wise
            size_t u = x - x%3; // the x index of the top left element of the current grid
            size_t v = y - y%3; // the y index of the top left element of the current grid
            for (size_t i = 0; i < 3; ++i)
            {
                for (size_t j = 0; j < 3; ++j)
                {
                    if (board[u + i][v + j] == digit)
                    {
                        return false;
                    }
                }
            }

            // If all conditions above are passed, then the element is valid
            return true;
        }

    private:
        std::vector<std::vector<char>> board_;
        bool isSolved_ = false;
    };

    class SumokuSolver
    {
    public:
        SumokuSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
        : board_(N, std::vector<size_t>(N, 0)),
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

        ~SumokuSolver() = default;

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
        /// @param x The current row index
        /// @param y The current column index
        /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
        virtual bool Backtrack(size_t x = 0, size_t y = 0)
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

        /// @brief Checks if an element is valid
        /// @param x The row index of the element
        /// @param y The column index of the element
        /// @param digit The digit of the element
        /// @return TRUE if the element is valid
        virtual bool Check(size_t x, size_t y, size_t digit)
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
            if ((isFilled && curSum ==  sum_[{x, y}]) || (!isFilled && curSum <  sum_[{x, y}]))
            {
                return true;
            }

            return false;
        }

    protected:
        std::unordered_map<Point, std::vector<Point>, PointHasher> adj_;
        SudokuBoard board_;
        size_t N_;
        bool solved_;
        std::unordered_map<Point, size_t, PointHasher> sum_;
    };

    class SumokuSolverWithBitMask : public SumokuSolver
    {
    public:
        SumokuSolverWithBitMask(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
        : SumokuSolver(N, boxes, sums), // let the base class constructor handle
        colMasks_(N, 0),
        rowMasks_(N, 0)
        {

        }

    private:
        /// @brief Solves the given Sudoku using backtracking technique
        /// @param x The current row index
        /// @param y The current column index
        /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
        bool Backtrack(size_t x = 0, size_t y = 0) override
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

        /// @brief Checks if an element is valid
        /// @param x The row index of the element
        /// @param y The column index of the element
        /// @param digit The digit of the element
        /// @return TRUE if the element is valid
        bool Check(size_t x, size_t y, size_t digit) override
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
            if ((isFilled && curSum ==  sum_[{x, y}]) || (!isFilled && curSum <  sum_[{x, y}]))
            {
                return true;
            }

            return false;
        }

    private:
        std::vector<uint16_t> colMasks_;
        std::vector<uint16_t> rowMasks_;
    };

    class SumokuOrdering
    {
    public:
        SumokuOrdering(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
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

    class SumokuOrderingWithBitMask : public SumokuOrdering
    {
    public:
        SumokuOrderingWithBitMask(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
        : SumokuOrdering(N, boxes, sums),
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

    class SumokuMRV
    {
    public:
        SumokuMRV(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
        : N_(N),
        board_(N * N, 0),
        boardView_(board_.data(), N, N),
        rowMask_(N, 0),
        colMask_(N, 0),
        boxMask_(boxes.size(), 0),
        boxID_(N, std::vector<size_t>(N, 0)),
        boxRemainingSum_(sums.begin(), sums.end()),
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

        void Solve()
        {
            solved_ = Backtrack();
        }

        [[nodiscard]] std::optional<SudokuBoard> GetSolution() const
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

        /// @brief Finds the next best cell that has the least candidates (most constrainted cell)
        /// @return The next best cell
        inline Selection FindNextBestCell()
        {
            Selection ret;
            int curMinCnt = static_cast<int>(N_) + 1;

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

                        // Early return if there is only a single candidate based on the box
                        if (std::popcount(sumMask) == 1)
                        {
                            return {.r = r, .c = c, .mask = candidates};
                        }

                        // If there is no candidate available that means we hit a dead end and this tree needs to be pruned
                        if (candidates == 0) [[unlikely]]
                        {
                            return Selection {.deadEnd = true};
                        }

                        #ifdef __GNUC__
                        int curNumOfCandidates = __builtin_popcount(candidates);
                        #else
                        int curNumOfCandidates = std::popcount(candidates);
                        #endif

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

        /// @brief Solves the given Sumoku using backtracking technique
        /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
        bool Backtrack()
        {
            Selection next = FindNextBestCell();

            // If the next best cell is illegal, that means backtracking fails
            if (next.deadEnd)
            {
                return false;
            }

            // If there is no next best cell and we are not hitting a dead end that means we have finished the entire board
            if (next.r == std::numeric_limits<size_t>::max() &&
    next.c == std::numeric_limits<size_t>::max())
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

        /// @brief Places a number on the board in a given cell
        /// @param r The row of the given cell
        /// @param c The column of the given cell
        /// @param digit The given number
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

        /// @brief Undoes a number on the board in a given cell (the exact opposite of what Place func does)
        /// @param r The row of the given cell
        /// @param c The column of the given cell
        /// @param digit The given number
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
        size_t N_;
        bool solved_ = false;
        std::vector<size_t> board_;
        std::mdspan<size_t, std::dextents<size_t, 2>> boardView_;
        std::vector<uint16_t> rowMask_, colMask_, boxMask_;
        SudokuBoard boxID_;
        std::vector<size_t> boxRemainingSum_;
        std::vector<size_t> boxRemainingCells_;
    };

    class KillerSudokuMRV
    {
    public:
        KillerSudokuMRV(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
        : N_(N),
        board_(N * N, 0),
        boardView_(board_.data(), N, N),
        rowMask_(N, 0),
        colMask_(N, 0),
        boxMask_(boxes.size(), 0),
        gridMask_((N / 3) * (N / 3), 0),
        boxID_(N, std::vector<size_t>(N, 0)),
        boxRemainingSum_(sums.begin(), sums.end()),
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

        void Solve()
        {
            solved_ = Backtrack();
        }

        [[nodiscard]] std::optional<SudokuBoard> GetSolution() const
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

    private:
        /// @brief The selection
        struct Selection
        {
            size_t r = -1;
            size_t c = -1;

            /// @brief The candidates in the mask form
            uint16_t mask = 0U;

            /// @brief TRUE if there is no other options
            bool deadEnd = false;
        };

        /// @brief Finds the next best cell that has the least candidates (most constrainted cell)
        /// @return The next best cell
        inline Selection FindNextBestCell()
        {
            Selection ret;
            int curMinCnt = static_cast<int>(N_) + 1;

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

                        #ifdef __GNUC__
                        int curNumOfCandidates = __builtin_popcount(candidates);
                        #else
                        int curNumOfCandidates = std::popcount(candidates);
                        #endif

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

        /// @brief Solves the given Sumoku using backtracking technique
        /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
        bool Backtrack()
        {
            Selection next = FindNextBestCell();

            // If the next best cell is illegal, that means backtracking fails
            if (next.deadEnd)
            {
                return false;
            }

            // If there is no next best cell and we are not hitting a dead end that means we have finished the entire board
            if (next.r == std::numeric_limits<size_t>::max() &&
    next.c == std::numeric_limits<size_t>::max())
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

        /// @brief Places a number on the board in a given cell
        /// @param r The row of the given cell
        /// @param c The column of the given cell
        /// @param digit The given number
        void Place(size_t r, size_t c, size_t digit)
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

        /// @brief Undoes a number on the board in a given cell (the exact opposite of what Place func does)
        /// @param r The row of the given cell
        /// @param c The column of the given cell
        /// @param digit The given number
        void Undo(size_t r, size_t c, size_t digit)
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

    private:
        size_t N_;
        bool solved_ = false;
        std::vector<size_t> board_;
        std::mdspan<size_t, std::dextents<size_t, 2>> boardView_;
        std::vector<uint16_t> rowMask_, colMask_, boxMask_, gridMask_;
        SudokuBoard boxID_;
        std::vector<size_t> boxRemainingSum_;
        std::vector<size_t> boxRemainingCells_;
    };
}   // solver

#endif // INCLUDE_SOLVER_SOLVERLIB_H_

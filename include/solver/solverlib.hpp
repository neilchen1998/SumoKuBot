#ifndef INCLUDE_SOLVER_SOLVERLIB_H_
#define INCLUDE_SOLVER_SOLVERLIB_H_

#include <algorithm>    // std::algorithm
#include <cstdint>  // uint16_t
#include <numeric>   // std::iota
#include <optional> // std::optional
#include <unordered_map>   // std::unordered_map
#include <vector>   // std::vector

#ifndef __GNUC__
#include <bit>  // std::popcount
#endif

#include <fmt/core.h>   // fmt::print

#include "board/boardlib.hpp"   // Point

namespace solver
{
    class SudokuSolver
    {
    public:
        SudokuSolver(std::vector<std::vector<char>>& board)
        : _board(board),
        _isSolved(false)
        {

        }

        void Solve()
        {
            _isSolved = Backtrack(_board);
        }

        std::optional<std::vector<std::vector<char>>> GetSolution() const
        {
            return _isSolved ? std::optional<std::vector<std::vector<char>>>{_board} : std::nullopt;
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
        std::vector<std::vector<char>> _board;
        bool _isSolved;
    };

    class SumokuSolver
    {
    public:
        SumokuSolver(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
        : _board(N, std::vector<int>(N, 0)),
        _N(N),
        _solved(false)
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
                            _adj[box[i]].emplace_back(box[j]);
                        }
                    }
                }
            }

            // Create the sum map
            for (size_t i = 0; i < boxes.size(); ++i)
            {
                for (auto& p : boxes[i])
                {
                    _sums[p] = sums[i];
                }
            }
        }

        ~SumokuSolver() = default;

        void Solve()
        {
            _solved = Backtrack();
        }

        std::optional<std::vector<std::vector<int>>> GetSolution() const
        {
            return _solved ? std::optional<std::vector<std::vector<int>>>{_board} : std::nullopt;
        }

        void PrintBoard() const
        {
            ::PrintBoard(_board);
        }

    protected:
        /// @brief Solves the given Sudoku using backtracking technique
        /// @param x The current row index
        /// @param y The current column index
        /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
        virtual bool Backtrack(size_t x = 0, size_t y = 0)
        {
            // If we reach the last column, then we start from the next row
            if (y == _N)
            {
                return Backtrack(x + 1, 0);
            }

            // If we reach to the end of the Sudoku board, then we have found a valid solution
            if (x == _N)
            {
                return true;
            }

            // If there is already a value on the current element, then we skip it
            if (_board[x][y] != 0)
            {
                return Backtrack(x, y + 1);
            }

            // We can put any number from 1 to _N
            for (size_t digit = 1; digit <= _N; ++digit)
            {
                // If the current guess is valid, then we write the current element with the guess
                if (Check(x, y, digit))
                {
                    _board[x][y] = digit;

                    // Trigger another backtrack
                    if (Backtrack(x, y + 1))
                    {
                        return true;
                    }

                    // The current guess is incorrect, we re-write it with a default value
                    // NOTE: if the guess were correct, then we would exit early and would not reach here
                    _board[x][y] = 0;
                }
            }

            return false;
        }

        /// @brief Checks if an element is valid
        /// @param x The row index of the element
        /// @param y The column index of the element
        /// @param digit The digit of the element
        /// @return TRUE if the element is valid
        virtual bool Check(size_t x, size_t y, int digit)
        {
            // Check if there is any duplicate row-wise
            for (size_t i = 0; i < _N; ++i)
            {
                if (_board[i][y] == digit)
                {
                    return false;
                }
            }

            // Check if there is any duplicate column-wise
            for (size_t j = 0; j < _N; ++j)
            {
                if (_board[x][j] == digit)
                {
                    return false;
                }
            }

            // Check if the box matches the sum
            bool isFilled = true;
            int curSum = digit;
            for (auto& [u, v] : _adj[{x, y}])
            {
                // As long as there is an element that is zero, that means the cage is not filled yet
                if (_board[u][v] == 0)
                {
                    isFilled = false;
                }
                curSum += _board[u][v];
            }

            // The cage restriction is met when:
            // 1. the cage has been filled and the current sum equals to the target sum
            // 2. the cage has not yet filled and the current sum is less than the target sum
            if ((isFilled && curSum ==  _sums[{x, y}]) || (!isFilled && curSum <  _sums[{x, y}]))
            {
                return true;
            }

            return false;
        }

    protected:
        std::unordered_map<Point, std::vector<Point>, PointHasher> _adj;
        std::vector<std::vector<int>> _board;
        size_t _N;
        bool _solved;
        std::unordered_map<Point, int, PointHasher> _sums;
    };

    class SumokuSolverWithBitMask : public SumokuSolver
    {
    public:
        SumokuSolverWithBitMask(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
        : SumokuSolver(N, boxes, sums), // let the base class constructor handle
        _colMasks(N, 0),
        _rowMasks(N, 0)
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
            if (y == _N)
            {
                return Backtrack(x + 1, 0);
            }

            // If we reach to the end of the Sudoku board, then we have found a valid solution
            if (x == _N)
            {
                return true;
            }

            // If there is already a value on the current element, then we skip it
            if (_board[x][y] != 0)
            {
                return Backtrack(x, y + 1);
            }

            // We can put any number from 1 to _N
            for (size_t digit = 1; digit <= _N; ++digit)
            {
                uint_fast16_t bit = (1U << digit);
                if (!(_rowMasks[x] & bit) && !(_colMasks[y] & bit))
                {
                    // If the current guess is valid, then we write the current element with the guess
                    if (Check(x, y, digit))
                    {
                        _board[x][y] = digit;
                        _rowMasks[x] |= bit;
                        _colMasks[y] |= bit;

                        // Trigger another backtrack
                        if (Backtrack(x, y + 1))
                        {
                            return true;
                        }

                        // The current guess is incorrect, we re-write it with a default value
                        // NOTE: if the guess were correct, then we would exit early and would not reach here
                        _board[x][y] = 0;
                        _rowMasks[x] &= ~bit;
                        _colMasks[y] &= ~bit;
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
        bool Check(size_t x, size_t y, int digit) override
        {
            // Check if the box matches the sum
            bool isFilled = true;
            int curSum = digit;
            for (auto& [u, v] : _adj[{x, y}])
            {
                // As long as there is an element that is zero, that means the cage is not filled yet
                if (_board[u][v] == 0)
                {
                    isFilled = false;
                }
                curSum += _board[u][v];
            }

            // The cage restriction is met when:
            // 1. the cage has been filled and the current sum equals to the target sum
            // 2. the cage has not yet filled and the current sum is less than the target sum
            if ((isFilled && curSum ==  _sums[{x, y}]) || (!isFilled && curSum <  _sums[{x, y}]))
            {
                return true;
            }

            return false;
        }

    private:
        std::vector<uint16_t> _colMasks;
        std::vector<uint16_t> _rowMasks;
    };

    class SumokuOrdering
    {
    public:
        SumokuOrdering(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
        : _board(N, std::vector<int>(N, 0)),
        _boxes(boxes),
        _N(N),
        _solved(false)
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
                            _boxMembers[box[i]].emplace_back(box[j]);
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
                return _boxes[lhs].size() < _boxes[rhs].size();
            });

            // Construct the visit order based on the size of the boxes
            for (size_t idx : indices)
            {
                _visitOrder.insert(_visitOrder.end(), _boxes[idx].begin(), _boxes[idx].end());
            }

            // Create the sum map
            for (size_t i = 0; i < boxes.size(); ++i)
            {
                for (auto& p : boxes[i])
                {
                    _sums[p] = sums[i];
                }
            }
        }

        void Solve()
        {
            _solved = Backtrack();
        }

        std::optional<std::vector<std::vector<int>>> GetSolution() const
        {
            return _solved ? std::optional<std::vector<std::vector<int>>>{_board} : std::nullopt;
        }

        void PrintBoard() const
        {
            ::PrintBoard(_board);
        }

    protected:
        /// @brief Solves the given Sudoku using backtracking technique
        /// @param idx The current visit index of the element
        /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
        virtual bool Backtrack(size_t idx = 0)
        {
            if (idx == _visitOrder.size())
            {
                return true;
            }

            Point p = _visitOrder[idx];
            for (size_t digit = 1; digit <= _N; ++digit)
            {
                if (IsValid(p.x, p.y, digit))
                {
                    _board[p.x][p.y] = digit;

                    if (Backtrack(idx + 1))
                    {
                        return true;
                    }

                    _board[p.x][p.y] = 0;
                }
            }

            return false;
        }

        /// @brief Checks if an element is valid
        /// @param x The row index of the element
        /// @param y The column index of the element
        /// @param digit The digit of the element
        /// @return TRUE if the element is valid
        virtual bool IsValid(size_t x, size_t y, int digit)
        {
            // Check the row and the column
            for (size_t i = 0; i < _N; ++i)
            {
                if ((_board[x][i] == digit) || (_board[i][y] == digit))
                {
                    return false;
                }
            }

            // Check the box
            int curSum = digit;
            bool isFull = true;

            for (const auto& member : _boxMembers[{x, y}])
            {
                if (member.x == x && member.y == y)
                {
                    continue;
                }

                int ele = _board[member.x][member.y];

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

            int target = _sums[{x, y}];

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
        std::vector<std::vector<int>> _board;
        std::vector<std::vector<Point>> _boxes;
        std::unordered_map<Point, std::vector<Point>, PointHasher> _boxMembers;
        size_t _N;
        std::unordered_map<Point, int, PointHasher> _sums;
        bool _solved;
        std::vector<Point> _visitOrder;
    };

    class SumokuOrderingWithBitMask : public SumokuOrdering
    {
    public:
        SumokuOrderingWithBitMask(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
        : SumokuOrdering(N, boxes, sums),
        _colMasks(N, 0),
        _rowMasks(N, 0)
        {
        }

    private:
        /// @brief Solves the given Sudoku using backtracking technique
        /// @param x The current row index
        /// @param y The current column index
        /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
        bool Backtrack(size_t idx = 0) override
        {
            if (idx == _visitOrder.size())
            {
                return true;
            }

            Point p = _visitOrder[idx];
            for (size_t digit = 1; digit <= _N; ++digit)
            {
                uint_fast16_t bit = (1U << digit);
                if (!(_rowMasks[p.x] & bit) && !(_colMasks[p.y] & bit))
                {
                    if (IsValid(p.x, p.y, digit))
                    {
                        _board[p.x][p.y] = digit;
                        _rowMasks[p.x] |= bit;
                        _colMasks[p.y] |= bit;

                        if (Backtrack(idx + 1))
                        {
                            return true;
                        }

                        _board[p.x][p.y] = 0;
                        _rowMasks[p.x] &= ~bit;
                        _colMasks[p.y] &= ~bit;
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
        bool IsValid(size_t x, size_t y, int digit) override
        {
            // Check the box
            int curSum = digit;
            bool isFull = true;

            for (const auto& member : _boxMembers[{x, y}])
            {
                if (member.x == x && member.y == y)
                {
                    continue;
                }

                int ele = _board[member.x][member.y];

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

            int target = _sums[{x, y}];

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
        std::vector<uint16_t> _colMasks;
        std::vector<uint16_t> _rowMasks;
    };

    class SumokuMRV
    {
    public:
        SumokuMRV(size_t N, const std::vector<std::vector<Point>>& boxes, const std::vector<int>& sums)
        : _N(N),
        _board(N, std::vector<int>(N, 0)),
        _rowMask(N, 0),
        _colMask(N, 0),
        _boxMask(boxes.size(), 0),
        _boxID(N, std::vector<size_t>(N, 0)),
        _boxRemainingSum(sums),
        _boxRemainingCells(sums.size(), 0),
        _options(N, std::vector<uint16_t>(N, 0))
        {
            for (size_t i = 0; i < boxes.size(); ++i)
            {
                _boxRemainingCells[i] = boxes[i].size();

                for (const Point& p : boxes[i])
                {
                    _boxID[p.x][p.y] = i;
                    _options[p.x][p.y] = GetPossibleNumbersMask(sums[i], boxes[i].size());
                }
            }
        }

        void Solve()
        {
            _solved = Backtrack();
        }

        std::optional<std::vector<std::vector<int>>> GetSolution() const
        {
            return _solved ? std::make_optional(_board) : std::nullopt;
        }

    private:

        /// @brief Find the possible number(s)
        /// @param r The current row
        /// @param c The current column
        /// @return Possible number(s) in the mask format
        uint16_t GetCandidates(size_t r, size_t c)
        {
            size_t id = _boxID[r][c];

            uint16_t forbidden = _rowMask[r] | _colMask[c] | (_boxMask[id] & ~(_options[r][c]));
            uint16_t ret = 0U;

            for (size_t v = 1; v <= _N; ++v)
            {
                // Check if the current number is possible
                if (!(forbidden & (1U << v)))
                {
                    int remainingSum = _boxRemainingSum[id] - v;

                    // If the current sum reaches to negative values,
                    // then the current number is not a possible
                    if (remainingSum < 0 )
                    {
                        continue;
                    }

                    // If there is only one cell left in the box and the remaining sum is not equal to zero,
                    // then we know that the current number is not possible either
                    if (_boxRemainingCells[id] == 1 && remainingSum != 0)
                    {
                        continue;
                    }

                    // If none of the two scenario is true, then the current nubmer is a possible candidate
                    ret |= (1U << v);
                }
            }

            return ret;
        }

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
            int curMinCnt = _N + 1;

            // Loop through the entire board to find the next best cell
            for (size_t r = 0; r < _N; ++r)
            {
                for (size_t c = 0; c < _N; ++c)
                {
                    // Only check the cell that is empty
                    if (_board[r][c] == 0)
                    {
                        // Get the candidates and the number of candidates
                        uint16_t candidates = GetCandidates(r, c);

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
            if (next.r == static_cast<size_t>(-1) && next.c == static_cast<size_t>(-1))
            {
                return true;
            }

            // Loop from number 1 to N
            for (size_t val = 1; val <= _N; ++val)
            {
                if (next.mask & (1U << val))
                {
                    Place(next.r, next.c, val);
                    if (Backtrack())
                    {
                        return true;
                    }
                    Undo(next.r, next.c, val);
                }
            }

            return false;
        }

        /// @brief Places a number on the board in a given cell
        /// @param r The row of the given cell
        /// @param c The column of the given cell
        /// @param val The given number
        void Place(size_t r, size_t c, int val)
        {
            size_t id = _boxID[r][c];

            _board[r][c] = val;
            _rowMask[r] |= (1U << val);
            _colMask[c] |= (1U << val);
            _boxMask[id] |= (1U << val);
            _options[r][c] &= ~(1U << val);
            _boxRemainingSum[id] -= val;
            --_boxRemainingCells[id];
        }

        /// @brief Undoes a number on the board in a given cell (the exact opposite of what Place func does)
        /// @param r The row of the given cell
        /// @param c The column of the given cell
        /// @param val The given number
        void Undo(size_t r, size_t c, int val)
        {
            size_t id = _boxID[r][c];

            _board[r][c] = 0;
            _rowMask[r] &= ~(1U << val);
            _colMask[c] &= ~(1U << val);
            _boxMask[id] &= ~(1U << val);
            _options[r][c]|= (1U << val);
            _boxRemainingSum[id] += val;
            ++_boxRemainingCells[id];
        }

    private:
        size_t _N;
        bool _solved = false;
        std::vector<std::vector<int>> _board;
        std::vector<uint16_t> _rowMask, _colMask, _boxMask;
        std::vector<std::vector<size_t>> _boxID;
        std::vector<int> _boxRemainingSum;
        std::vector<size_t> _boxRemainingCells;
        std::vector<std::vector<uint16_t>> _options;
    };
}   // solver

#endif // INCLUDE_SOLVER_SOLVERLIB_H_

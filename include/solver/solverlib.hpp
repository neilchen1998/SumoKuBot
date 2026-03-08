#ifndef INCLUDE_SOLVER_SOLVERLIB_H_
#define INCLUDE_SOLVER_SOLVERLIB_H_

#include <algorithm>    // std::algorithm
#include <cstdint>  // uint16_t
#include <numeric>   // std::iota
#include <optional> // std::optional
#include <unordered_map>   // std::unordered_map
#include <vector>   // std::vector

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
        /// @param val The value of the element
        /// @return TRUE if the element is valid
        bool Check(const std::vector<std::vector<char>>& board, size_t x, size_t y, char val)
        {
            // Check if there is any duplicate row-wise
            for (size_t i = 0; i < 9; ++i)
            {
                if (board[i][y] == val)
                {
                    return false;
                }
            }

            // Check if there is any duplicate column-wise
            for (size_t j = 0; j < 9; ++j)
            {
                if (board[x][j] == val)
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
                    if (board[u + i][v + j] == val)
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
            for (int c = 1; c <= _N; ++c)
            {
                // If the current guess is valid, then we write the current element with the guess
                if (Check(x, y, c))
                {
                    _board[x][y] = c;

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
        /// @param val The value of the element
        /// @return TRUE if the element is valid
        virtual bool Check(size_t x, size_t y, int val)
        {
            // Check if there is any duplicate row-wise
            for (size_t i = 0; i < _N; ++i)
            {
                if (_board[i][y] == val)
                {
                    return false;
                }
            }

            // Check if there is any duplicate column-wise
            for (size_t j = 0; j < _N; ++j)
            {
                if (_board[x][j] == val)
                {
                    return false;
                }
            }

            // Check if the box matches the sum
            bool isFilled = true;
            int curSum = val;
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
        _rowMasks(N, 0),
        _colMasks(N, 0)
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
            for (int c = 1; c <= _N; ++c)
            {
                uint_fast16_t bit = (1U << c);
                if (!(_rowMasks[x] & bit) && !(_colMasks[y] & bit))
                {
                    // If the current guess is valid, then we write the current element with the guess
                    if (Check(x, y, c))
                    {
                        _board[x][y] = c;
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
        /// @param val The value of the element
        /// @return TRUE if the element is valid
        bool Check(size_t x, size_t y, int val) override
        {
            // Check if the box matches the sum
            bool isFilled = true;
            int curSum = val;
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
        _N(N),
        _boxes(boxes),
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
        /// @param x The current row index
        /// @param y The current column index
        /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
        virtual bool Backtrack(size_t idx = 0)
        {
            if (idx == _visitOrder.size())
            {
                return true;
            }

            Point p = _visitOrder[idx];
            for (int val = 1; val <= _N; ++val)
            {
                if (IsValid(p.x, p.y, val))
                {
                    _board[p.x][p.y] = val;

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
        /// @param val The value of the element
        /// @return TRUE if the element is valid
        virtual bool IsValid(size_t x, size_t y, int val)
        {
            // Check the row and the column
            for (size_t i = 0; i < _N; ++i)
            {
                if ((_board[x][i] == val) || (_board[i][y] == val))
                {
                    return false;
                }
            }

            // Check the box
            int curSum = val;
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
                else if (ele == val)
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
            for (int val = 1; val <= _N; ++val)
            {
                uint_fast16_t bit = (1U << val);
                if (!(_rowMasks[p.x] & bit) && !(_colMasks[p.y] & bit))
                {
                    if (IsValid(p.x, p.y, val))
                    {
                        _board[p.x][p.y] = val;
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
        /// @param val The value of the element
        /// @return TRUE if the element is valid
        bool IsValid(size_t x, size_t y, int val) override
        {
            // Check the box
            int curSum = val;
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
                else if (ele == val)
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
}   // solver

#endif // INCLUDE_SOLVER_SOLVERLIB_H_

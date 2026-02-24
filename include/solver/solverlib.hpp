#ifndef INCLUDE_SOLVER_SOLVERLIB_H_
#define INCLUDE_SOLVER_SOLVERLIB_H_

#include <optional> // std::optional
#include <unordered_map>   // std::unordered_map
#include <vector>   // std::vector

#include <fmt/core.h>   // fmt::print

#include "board/boardlib.hpp"   // Point

#define SUMOKU_SIZE 3

namespace solver
{
    class SudokuSolver
    {
    public:
        SudokuSolver(std::vector<std::vector<char>>& board)
        {
            Backtrack(board);
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

            Solve();
        }

        void Solve()
        {
            _solved = Backtrack(_board);
        }

        std::optional<std::vector<std::vector<int>>> GetSolution() const
        {
            return _solved ? std::optional<std::vector<std::vector<int>>>{_board} : std::nullopt;
        }

        void PrintBoard() const
        {
            ::PrintBoard(_board);
        }

    private:
        /// @brief Solves the given Sudoku using backtracking technique
        /// @param board The Sudoku board
        /// @param x The current row index
        /// @param y The current column index
        /// @return TRUE if a valid solution is found from the current state, FALSE if no valid solution exists, triggering a backtrack
        bool Backtrack(std::vector<std::vector<int>>& board, size_t x = 0, size_t y = 0)
        {
            // If we reach the last column, then we start from the next row
            if (y == _N)
            {
                return Backtrack(board, x + 1, 0);
            }

            // If we reach to the end of the Sudoku board, then we have found a valid solution
            if (x == _N)
            {
                return true;
            }

            // If there is already a value on the current element, then we skip it
            if (board[x][y] != 0)
            {
                return Backtrack(board, x, y + 1);
            }

            // We can put any number from 1 to _N
            for (int c = 1; c <= _N; ++c)
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
                    board[x][y] = 0;
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
        bool Check(const std::vector<std::vector<int>>& board, size_t x, size_t y, int val)
        {
            // Check if there is any duplicate row-wise
            for (size_t i = 0; i < _N; ++i)
            {
                if (board[i][y] == val)
                {
                    return false;
                }
            }

            // Check if there is any duplicate column-wise
            for (size_t j = 0; j < _N; ++j)
            {
                if (board[x][j] == val)
                {
                    return false;
                }
            }

            // Check if the box matches the sum
            int curSum = val;
            for (auto& [u, v] : _adj[{x, y}])
            {
                curSum += _board[u][v];
            }

            if (curSum > _sums[{x, y}])
            {
                return false;
            }

            // If all conditions above are passed, then the element is valid
            return true;
        }

    private:
        std::unordered_map<Point, std::vector<Point>, PointHasher> _adj;
        std::vector<std::vector<int>> _board;
        size_t _N;
        bool _solved;
        std::unordered_map<Point, int, PointHasher> _sums;
    };
}   // solver

#endif // INCLUDE_SOLVER_SOLVERLIB_H_

#include "sudokubacktracking.hpp"

#include <cstddef>  // size_t
#include <optional> // std::optional
#include <utility>  // std::move

#include "board/boardlib.hpp" // Point, SudokuBoard, SUDOKU_SZ

namespace sudoku
{
SudokuBacktracking::SudokuBacktracking(SudokuBoard board) : board_(std::move(board))
{
}

void SudokuBacktracking::Solve()
{
    isSolved_ = Backtrack(board_);
}

std::optional<SudokuBoard> SudokuBacktracking::GetSolution() const
{
    return isSolved_ ? std::optional<SudokuBoard> {board_} : std::nullopt;
}

bool SudokuBacktracking::Backtrack(SudokuBoard& board, size_t x, size_t y)
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
    if (board[x][y] != 0)
    {
        return Backtrack(board, x, y + 1);
    }

    // We can put any number from 1 to 9
    for (int c = 1; c <= 9; ++c)
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

bool SudokuBacktracking::Check(const SudokuBoard& board, size_t x, size_t y, size_t digit)
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
    size_t u = x - x % 3; // the x index of the top left element of the current grid
    size_t v = y - y % 3; // the y index of the top left element of the current grid
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
} // namespace sudoku

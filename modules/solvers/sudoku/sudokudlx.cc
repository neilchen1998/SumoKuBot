#include "sudokudlx.hpp"

#include <cstddef>  // size_t
#include <memory.h> // std::make_unique, std::unique_ptr

#include "board/boardlib.hpp" // Point, SudokuBoard, SUDOKU_SZ

namespace sudoku
{
SudokuDLXSolver::SudokuDLXSolver(const SudokuBoard& board)
    : _board(board), _root(std::make_unique<ColumnHeader>(-1)), _columns(TOTAL_NUM_OF_CONSTRAINTS)
{
    _root->left = _root.get();
    _root->right = _root.get();

    for (size_t i = 0; i < TOTAL_NUM_OF_CONSTRAINTS; ++i)
    {
        _columns[i] = std::make_unique<ColumnHeader>(static_cast<int>(i));

        ColumnHeader* column = _columns[i].get();

        // Insert the column immediately before the root.
        column->left = _root->left;
        column->right = _root.get();

        _root->left->right = column;
        _root->left = column;
    }

    // Generate the constraint matrix
    for (int r = 0; r < 9; ++r)
    {
        for (int c = 0; c < 9; ++c)
        {
            const int cellDigit = _board[r][c];

            if (cellDigit > 0 && cellDigit <= 9)
            {
                AddRow(r, c, cellDigit, CalculateSudokuConstraints(r, c, cellDigit));
            }
            else
            {
                for (int digit = 1; digit <= 9; ++digit)
                {
                    AddRow(r, c, digit, CalculateSudokuConstraints(r, c, digit));
                }
            }
        }
    }
}

/// @brief Solves the Sudoku puzzle
void SudokuDLXSolver::Solve()
{
    if (Search())
    {
        // Construct the solution
        for (Node* ptr : _rows)
        {
            _board[ptr->r][ptr->c] = ptr->d;
        }

        isSolved_ = true;
    }
    else
    {
        isSolved_ = false;
    }
}

/// @brief Gets the solution
/// @return The solved Sudoku board
[[nodiscard]] std::optional<SudokuBoard> SudokuDLXSolver::GetSolution() const
{
    return isSolved_ ? std::optional<SudokuBoard> {_board} : std::nullopt;
}

/// @brief Search the solution using MRV heuristic
/// @return true if the matrix is completed recovered, false if there is a dead end
bool SudokuDLXSolver::Search()
{
    // Check if the matrix is completely recovered
    if (_root->right == _root.get())
    {
        return true;
    }

    // Loop through all columns and find the one that has the least number of nodes
    ColumnHeader* col = dynamic_cast<ColumnHeader*>(_root->right);
    for (Node* cur = _root->right; cur != _root.get(); cur = cur->right)
    {
        ColumnHeader* colHeader = dynamic_cast<ColumnHeader*>(cur);
        if (colHeader->size < col->size)
        {
            col = colHeader;
        }
    }

    // Dead end
    if (col->size == 0)
    {
        return false;
    }

    Cover(col);

    for (Node* row = col->down; row != col; row = row->down)
    {
        _rows.push_back(row);

        for (Node* node = row->right; node != row; node = node->right)
        {
            Cover(node->header);
        }

        if (Search())
        {
            return true;
        }

        // Undo
        _rows.pop_back();

        for (Node* node = row->left; node != row; node = node->left)
        {
            Uncover(node->header);
        }
    }

    Uncover(col);

    // If we reach here that means we have exhausted all the possibilities
    // and therefore no feasible solution
    return false;
}

/// @brief Add a new row
/// @param r The row index
/// @param c The column index
/// @param d The digit
/// @param col_indices The indices of columns
void SudokuDLXSolver::AddRow(int r, int c, int d, const SudokuConstraints& col_indices) noexcept
{
    // Create a pointer to the very first node of the row
    Node* firstNode = nullptr;

    // Loop through all the column indices
    for (const int idx : col_indices)
    {
        // Get the column header
        ColumnHeader* col = _columns[idx].get();

        // Create a new node
        _nodes.push_back(std::make_unique<Node>(col, r, c, d));

        // Obtain a non-owning pointer to the node
        Node* node = _nodes.back().get();

        // Link the newly created node to the column header (vertically)
        node->down = col;
        node->up = col->up;
        col->up->down = node;
        col->up = node;
        ++col->size;

        // Link the newly created node to the end of the row (horizontally)
        if (!firstNode)
        {
            firstNode = node;
        }
        else
        {
            node->left = firstNode->left;
            node->right = firstNode;
            firstNode->left->right = node;
            firstNode->left = node;
        }
    }
}

/// @brief Detaches the given column from its neighbours
/// @param col The given column
void SudokuDLXSolver::Cover(ColumnHeader* col)
{
    // Detach its neighbours
    col->left->right = col->right;
    col->right->left = col->left;

    // Go down each row
    for (Node* row = col->down; row != col; row = row->down)
    {
        // Go through each column (iterate to the right)
        for (Node* node = row->right; node != row; node = node->right)
        {
            node->down->up = node->up;
            node->up->down = node->down;
            --node->header->size;
        }
    }
}

/// @brief Attaches the given column from its neighbours
/// @param col The given column
void SudokuDLXSolver::Uncover(ColumnHeader* col)
{
    // Go up each row
    for (Node* row = col->up; row != col; row = row->up)
    {
        // Go through each column (iterate to the left)
        for (Node* node = row->left; node != row; node = node->left)
        {
            node->down->up = node;
            node->up->down = node;
            ++node->header->size;
        }
    }

    // Attach its neighbours
    col->right->left = col;
    col->left->right = col;
}

/// @brief Gets the box ID of a given cell
/// @param r The row of the cell
/// @param c The column of the cell
[[nodiscard]] constexpr int SudokuDLXSolver::GetBoxID(const int r, const int c) const noexcept
{
    return (r / 3) * 3 + (c / 3);
}

/// @brief Calculates the constraints
/// @param r The row index
/// @param c The column index
/// @param d The digit
/// @return The constraints
[[nodiscard]] constexpr SudokuDLXSolver::SudokuConstraints SudokuDLXSolver::CalculateSudokuConstraints(int r, int c, int d) const noexcept
{
    // Get the digit index (0-based)
    const int digit = d - 1;
    const int box = GetBoxID(r, c);

    return {
        r * 9 + c,            // cell constraints (81 cells in total)
        81 + r * 9 + digit,   // row constraints (9 rows x 9 digits = 81 in total)
        162 + c * 9 + digit,  // columns constraints (9 columns x 9 digits = 81 in total)
        243 + box * 9 + digit // box constraints (9 boxes x 9 digits = 81 in total)
    };
}
} // namespace sudoku

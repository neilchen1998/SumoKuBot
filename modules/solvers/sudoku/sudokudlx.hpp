#ifndef MODULES_SOLVERS_SUDOKU_SUDOKUDLX_HPP
#define MODULES_SOLVERS_SUDOKU_SUDOKUDLX_HPP

#include <cstddef>  // size_t
#include <memory.h> // std::make_unique, std::unique_ptr
#include <optional> // std::optional
#include <vector>   // std::vector

#include "board/boardlib.hpp" // Point, SudokuBoard, SUDOKU_SZ

namespace sudoku
{

class SudokuDLXSolver
{
  public:
    SudokuDLXSolver(SudokuBoard board);
    ~SudokuDLXSolver() = default;

    SudokuDLXSolver(const SudokuDLXSolver&) = delete;
    SudokuDLXSolver& operator=(const SudokuDLXSolver&) = delete;
    SudokuDLXSolver(SudokuDLXSolver&&) = delete;
    SudokuDLXSolver& operator=(SudokuDLXSolver&&) = delete;

    /// @brief Solves the Sudoku puzzle
    void Solve();

    /// @brief Gets the solution
    /// @return The solved Sudoku board
    [[nodiscard]] std::optional<SudokuBoard> GetSolution() const;

  private:
    static constexpr size_t TOTAL_NUM_OF_CONSTRAINTS = (81 + (SUDOKU_SZ * SUDOKU_SZ) + (SUDOKU_SZ * SUDOKU_SZ) + (SUDOKU_SZ * SUDOKU_SZ));
    using SudokuConstraints = std::array<int, 4>;

    // Forward declaration
    struct ColumnHeader;

    // Circular doubly-linked list
    struct Node
    {
        // Pointers
        Node* left = this;
        Node* right = this;
        Node* up = this;
        Node* down = this;
        ColumnHeader* header = nullptr;

        /// @brief Row index
        int r = -1;

        /// @brief Column index
        int c = -1;

        /// @brief Digit
        int d = -1;

        Node() = default;

        Node(ColumnHeader* header, int row, int col, int digit) : header(header), r(row), c(col), d(digit)
        {
        }

        virtual ~Node() = default;

        // Delete copying & move semantics
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;
        Node(Node&&) noexcept = delete;
        Node& operator=(Node&&) = delete;
    };

    /// @brief Column header node
    struct ColumnHeader : public Node
    {
        /// @brief The id
        unsigned int id = -1;

        /// @brief The number of nodes under the column
        size_t size = 0;

        ColumnHeader(int id) : id(id)
        {
            header = this;
        }
    };

    /// @brief Search the solution using MRV heuristic
    /// @return true if the matrix is completed recovered, false if there is a dead end
    bool Search();

    /// @brief Add a new row
    /// @param r The row index
    /// @param c The column index
    /// @param d The digit
    /// @param col_indices The indices of columns
    void AddRow(int r, int c, int d, const SudokuConstraints& col_indices) noexcept;

    /// @brief Detaches the given column from its neighbours
    /// @param col The given column
    void Cover(ColumnHeader* col);

    /// @brief Attaches the given column from its neighbours
    /// @param col The given column
    void Uncover(ColumnHeader* col);

    /// @brief Gets the box ID of a given cell
    /// @param r The row of the cell
    /// @param c The column of the cell
    [[nodiscard]] constexpr int GetBoxID(const int r, const int c) const noexcept;

    /// @brief Calculates the constraints
    /// @param r The row index
    /// @param c The column index
    /// @param d The digit
    /// @return The constraints
    [[nodiscard]] constexpr SudokuConstraints CalculateSudokuConstraints(int r, int c, int d) const noexcept;

  private:
    /// @brief The board
    SudokuBoard _board;

    /// @brief The root of the column header
    std::unique_ptr<ColumnHeader> _root;

    /// @brief The columns in the matrix that represents constraints (owning)
    std::vector<std::unique_ptr<ColumnHeader>> _columns;

    /// @brief The rows in the matrix that represents choices (not owning)
    std::vector<Node*> _rows;

    /// @brief All the nodes in the matrix (owning)
    std::vector<std::unique_ptr<Node>> _nodes;

    /// @brief True if the puzzle is solved
    bool isSolved_ = false;
};

} // namespace sudoku

#endif // MODULES_SOLVERS_SUDOKU_SUDOKUDLX_HPP

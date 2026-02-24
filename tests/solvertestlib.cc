#define CATCH_CONFIG_MAIN

#include <numeric>  // std::iota
#include <vector>    // std::vector
#include <ranges>    // std::ranges::input_range
#include <unordered_set>    // std::unordered_set
#include <catch2/catch_test_macros.hpp> // TEST_CASE, SECTION, REQUIRE
#include <catch2/matchers/catch_matchers_all.hpp>   // Catch::Matchers::Equals

#include "solver/solverlib.hpp"

/// @brief Validates a given board is a square board
/// @tparam T The element type of the board
/// @param board The board
template <typename T>
void validate_boad_is_square(const std::vector<std::vector<T>>& board)
{
    const size_t N = board.size();

    for (const auto& row : board)
    {
        REQUIRE (row.size() == N);
    }
}

/// @brief Validates a given Sukodu satisfy both the row and column constraints
/// @tparam T The element type of the board
/// @param board The Sudoku or any variations
template <typename T>
void validate_sukodu_row_column_constraints(const std::vector<std::vector<T>>& board)
{
    const size_t N = board.size();

    std::vector<int> expected_vec(N);
    std::iota(expected_vec.begin(), expected_vec.end(), 1);

    for (size_t i = 0; i < N; ++i)
    {
        std::vector<int> row, col;
        for (size_t j = 0; j < N; ++j)
        {
            row.emplace_back(board[i][j]);
            col.emplace_back(board[j][i]);
        }

        REQUIRE_THAT(row, Catch::Matchers::UnorderedEquals(expected_vec));
        REQUIRE_THAT(col, Catch::Matchers::UnorderedEquals(expected_vec));
    }
}

TEST_CASE( "Sukodu", "[main]" )
{
    SECTION("Puzzle 0", "[trivial case]")
    {
        std::vector<std::vector<char>> board ({{'5','3','.','.','7','.','.','.','.'},{'6','.','.','1','9','5','.','.','.'},{'.','9','8','.','.','.','.','6','.'},{'8','.','.','.','6','.','.','.','3'},{'4','.','.','8','.','3','.','.','1'},{'7','.','.','.','2','.','.','.','6'},{'.','6','.','.','.','.','2','8','.'},{'.','.','.','4','1','9','.','.','5'},{'.','.','.','.','8','.','.','7','9'}});
        std::vector<std::vector<char>> ans ({{'5','3','4','6','7','8','9','1','2'},{'6','7','2','1','9','5','3','4', '8'},{'1','9','8','3','4','2','5','6','7'},{'8','5','9','7','6','1','4','2','3'},{'4','2','6','8','5','3','7','9','1'},{'7','1','3','9','2','4','8','5','6'},{'9','6','1','5','3','7','2','8','4'},{'2','8','7','4','1','9','6','3','5'},{'3','4','5','2','8','6','1','7','9'}});

        solver::SudokuSolver s {board};

        REQUIRE (board.size() == 9);

        for (size_t row = 0; row < board.size(); ++row)
        {
            INFO ("Failing at row: " << row);
            CHECK (board[row] == ans[row]);
        }
    }
}

TEST_CASE( "Sumoku", "[main]" )
{
    SECTION("Puzzle 0", "[2 x 2]")
    {
        constexpr size_t N = 2;
        const std::vector<std::vector<Point>> boxes {{{0, 0}, {0, 1}}, {{1, 0}, {1, 1}}};
        const std::vector<int> sums {3, 3};

        solver::SumokuSolver s {N, boxes, sums};

        auto ret = s.GetSolution();
        REQUIRE (ret != std::nullopt);

        std::vector<std::vector<int>> solution = *ret;

        REQUIRE (solution.size() == N);
        validate_boad_is_square(solution);
        validate_sukodu_row_column_constraints(solution);
    }

    SECTION("Puzzle 1", "[3 x 3]")
    {
        constexpr size_t N = 3;
        const std::vector<std::vector<Point>> boxes {{{0, 0}, {1, 0}}, {{0, 1}, {0, 2}}, {{1, 1}, {1, 2}}, {{2, 0}, {2, 1}}, {{2, 2}}};
        const std::vector<int> sums {3, 5, 4, 5, 1};

        solver::SumokuSolver s {N, boxes, sums};

        auto ret = s.GetSolution();
        REQUIRE (ret != std::nullopt);

        std::vector<std::vector<int>> solution = *ret;

        REQUIRE (solution.size() == N);
        validate_boad_is_square(solution);
        validate_sukodu_row_column_constraints(solution);
    }
}

#define CATCH_CONFIG_MAIN

#include <algorithm>  // std::ranges::all_of
#include <concepts> // concept
#include <numeric>  // std::iota
#include <ranges>    // std::ranges::input_range
#include <unordered_set>    // std::unordered_set
#include <vector>    // std::vector

#include <catch2/catch_test_macros.hpp> // TEST_CASE, SECTION, REQUIRE
#include <catch2/matchers/catch_matchers_all.hpp>   // Catch::Matchers::Equals

#include "solver/solverlib.hpp"

/// @brief Converts a character digit or integer to an int
/// @tparam T Must be char or int
/// @param v The given value to be converted
/// @return int The result
template <typename T>
requires std::same_as<T, char> || std::integral<T>
int to_int(T v)
{
    if constexpr (std::is_same_v<T, char>)
    {
        return v - '0';
    }
    else
    {
        return static_cast<int>(v);
    }
}

/// @brief Validates a given board is a square board
/// @tparam T The element type of the board
/// @param board The board
template <BoardType T>
void validate_boad_is_square(const std::vector<std::vector<T>>& board)
{
    REQUIRE_FALSE(board.empty());

    const size_t N = board.size();

    const bool isSquare = std::ranges::all_of(board, [N](const auto& row)
    {
        return row.size() == N;
    });

    REQUIRE (isSquare);
}

/// @brief Validates a given Sukodu satisfy both the row and column constraints
/// @tparam T The element type of the board
/// @param board The Sudoku or any variations
template <BoardType T>
void validate_sukodu_row_column_constraints(const std::vector<std::vector<T>>& board)
{
    const size_t N = board.size();

    std::vector<int> expected_vec(N);
    std::iota(expected_vec.begin(), expected_vec.end(), 1);

    for (size_t i = 0; i < N; ++i)
    {
        std::vector<int> row, col;
        row.reserve(N);
        col.reserve(N);

        for (size_t j = 0; j < N; ++j)
        {
            row.emplace_back(to_int(board[i][j]));
            col.emplace_back(to_int(board[j][i]));
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

        s.Solve();

        auto ret = s.GetSolution();
        REQUIRE (ret != std::nullopt);

        std::vector<std::vector<char>> solution = *ret;

        REQUIRE (solution.size() == ans.size());
        validate_boad_is_square(solution);
        validate_sukodu_row_column_constraints(solution);
    }
}

TEST_CASE( "Sumoku (SumokuSolver)", "[main]" )
{
    SECTION("Puzzle 0", "[2 x 2]")
    {
        constexpr size_t N = 2;
        const std::vector<std::vector<Point>> boxes {{{0, 0}, {0, 1}}, {{1, 0}, {1, 1}}};
        const std::vector<int> sums {3, 3};

        solver::SumokuSolver s {N, boxes, sums};

        s.Solve();

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

        s.Solve();

        auto ret = s.GetSolution();
        REQUIRE (ret != std::nullopt);

        std::vector<std::vector<int>> solution = *ret;

        REQUIRE (solution.size() == N);
        validate_boad_is_square(solution);
        validate_sukodu_row_column_constraints(solution);
    }

    SECTION("Puzzle 2", "[4 x 4]")
    {
        constexpr size_t N = 4;
        const std::vector<std::vector<Point>> boxes {{{0, 0}, {1, 0}, {2, 0}}, {{0, 1}}, {{0, 2}, {0, 3}}, {{1, 1}, {2, 1}}, {{1, 2}}, {{1, 3}, {2, 3}, {3, 3}}, {{2, 2}, {3, 2}}, {{3, 0}, {3, 1}}};
        const std::vector<int> sums {6, 4, 5, 5, 1, 8, 6, 5};

        solver::SumokuSolver s {N, boxes, sums};

        s.Solve();

        auto ret = s.GetSolution();
        REQUIRE (ret != std::nullopt);

        std::vector<std::vector<int>> solution = *ret;

        REQUIRE (solution.size() == N);
        validate_boad_is_square(solution);
        validate_sukodu_row_column_constraints(solution);
    }

    SECTION("Puzzle 3", "[9 x 9]")
    {
        constexpr size_t N = 9;
        const std::vector<std::vector<Point>> boxes {
            {{0, 0}, {0, 1}}, {{0, 2}, {1, 2}}, {{0 ,3}, {0, 4}}, {{0, 5}, {1, 5}}, {{0, 6}, {0, 7}}, {{0, 8}, {1, 8}},
            {{1, 0}, {1, 1}}, {{1, 3}, {1, 4}}, {{1, 6}, {1, 7}},
            {{2, 0}, {3, 0}}, {{2, 1}, {3, 1}}, {{2, 2}, {3, 2}}, {{2, 3}, {3, 3}}, {{2, 4}, {3, 4}}, {{2, 5}, {2, 6}}, {{2, 7}, {3, 7}}, {{2, 8}, {3, 8}},
            {{3, 5}, {3, 6}},
            {{4, 0}, {4, 1}}, {{4, 2}, {4, 3}}, {{4, 4}, {4, 5}}, {{4, 6}, {5, 6}}, {{4, 7}, {4, 8}},
            {{5, 0}, {5, 1}}, {{5, 2}, {5, 3}}, {{5, 4}, {5, 5}}, {{5, 7}, {5, 8}},
            {{6, 0}, {6, 1}}, {{6, 2}, {6, 3}}, {{6, 4}, {6, 5}}, {{6, 6}, {7, 6}}, {{6, 7}, {6, 8}},
            {{7, 0}, {8, 0}}, {{7, 1}, {7, 2}}, {{7, 3}, {8, 3}}, {{7, 4}, {7, 5}}, {{7, 7}, {8, 7}}, {{7, 8}, {8, 8}},
            {{8, 1}, {8, 2}}, {{8, 4}, {8, 5}, {8, 6}}
        };
        const std::vector<int> sums {
            15, 13, 10, 5, 8, 8,
            12, 6, 13,
            15, 6, 4, 5, 13, 12, 11, 13,
            11,
            9, 11, 13, 3, 10,
            7, 13, 17, 7,
            10, 7, 10, 12, 11,
            9, 11, 14, 11, 10, 8,
            9, 13
        };

        solver::SumokuSolver s {N, boxes, sums};

        s.Solve();

        auto ret = s.GetSolution();
        REQUIRE (ret != std::nullopt);

        std::vector<std::vector<int>> solution = *ret;

        REQUIRE (solution.size() == N);
        validate_boad_is_square(solution);
        validate_sukodu_row_column_constraints(solution);
    }
}

#define CATCH_CONFIG_MAIN

#include <vector>    // std::vector
#include <ranges>    // std::ranges::input_range
#include <unordered_set>    // std::unordered_set
#include <catch2/catch_test_macros.hpp> // TEST_CASE, SECTION, REQUIRE
#include <catch2/matchers/catch_matchers_all.hpp>   // Catch::Matchers::Equals

#include "solver/solverlib.hpp"

TEST_CASE( "Sukodu", "[main]" )
{
    SECTION("Puzzle 0", "[trivial case]")
    {
        std::vector<std::vector<char>> board ({{'5','3','.','.','7','.','.','.','.'},{'6','.','.','1','9','5','.','.','.'},{'.','9','8','.','.','.','.','6','.'},{'8','.','.','.','6','.','.','.','3'},{'4','.','.','8','.','3','.','.','1'},{'7','.','.','.','2','.','.','.','6'},{'.','6','.','.','.','.','2','8','.'},{'.','.','.','4','1','9','.','.','5'},{'.','.','.','.','8','.','.','7','9'}});
        std::vector<std::vector<char>> ans ({{'5','3','4','6','7','8','9','1','2'},{'6','7','2','1','9','5','3','4', '8'},{'1','9','8','3','4','2','5','6','7'},{'8','5','9','7','6','1','4','2','3'},{'4','2','6','8','5','3','7','9','1'},{'7','1','3','9','2','4','8','5','6'},{'9','6','1','5','3','7','2','8','4'},{'2','8','7','4','1','9','6','3','5'},{'3','4','5','2','8','6','1','7','9'}});

        solver::Solver s {board};

        REQUIRE (board.size() == 9);

        for (size_t row = 0; row < board.size(); ++row)
        {
            INFO ("Failing at row: " << row);
            CHECK (board[row] == ans[row]);
        }
    }
}

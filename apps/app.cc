#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <vector>  // std::vector

#include <fmt/core.h>   // fmt::print

#include "board/boardlib.hpp"
#include "solver/solverlib.hpp"

int main(int argc, char* argv[])
{
    std::vector<std::vector<char>> board ({{'5','3','.','.','7','.','.','.','.'},{'6','.','.','1','9','5','.','.','.'},{'.','9','8','.','.','.','.','6','.'},{'8','.','.','.','6','.','.','.','3'},{'4','.','.','8','.','3','.','.','1'},{'7','.','.','.','2','.','.','.','6'},{'.','6','.','.','.','.','2','8','.'},{'.','.','.','4','1','9','.','.','5'},{'.','.','.','.','8','.','.','7','9'}});

    solver::Solver s {board};

    PrintBoard(board);

    return EXIT_SUCCESS;
}

add_library(sudoku_solver sudokumrv.cc)

    target_include_directories(sudoku_solver PUBLIC $ {CMAKE_CURRENT_SOURCE_DIR})

        target_link_libraries(sudoku_solver PUBLIC board_library math_library)

            add_library(sudoku::solver ALIAS sudoku_solver)

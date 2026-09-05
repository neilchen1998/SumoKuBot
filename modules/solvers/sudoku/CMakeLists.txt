add_library(sudoku_solver sudokubacktracking.cc sudokudlx.cc sudokumrv.cc)

target_include_directories(sudoku_solver PUBLIC ${PROJECT_SOURCE_DIR}/modules)

target_link_libraries(sudoku_solver PUBLIC board_library)

add_library(sudoku::solver ALIAS sudoku_solver)

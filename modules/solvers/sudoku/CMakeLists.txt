add_library(sudoku_solver sudokubacktracking.cc sudokudlx.cc sudokumrv.cc)
add_library(killer_sudoku_solver killersudokumrvsolver.cc)

target_include_directories(sudoku_solver PUBLIC ${PROJECT_SOURCE_DIR}/modules)
target_include_directories(killer_sudoku_solver PUBLIC ${PROJECT_SOURCE_DIR}/modules)

target_link_libraries(sudoku_solver PUBLIC board_library)
target_link_libraries(killer_sudoku_solver PUBLIC board_library)

add_library(sudoku::solver ALIAS sudoku_solver)
add_library(killer_sudoku::solver ALIAS killer_sudoku_solver)

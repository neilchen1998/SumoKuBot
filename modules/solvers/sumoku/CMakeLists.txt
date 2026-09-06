add_library(sumoku_solver sumokubacktracking.cc sumokubacktrackingbitmask.cc)

target_include_directories(sumoku_solver PUBLIC ${PROJECT_SOURCE_DIR}/modules)

target_link_libraries(sumoku_solver PUBLIC board_library)

add_library(sumoku::solver ALIAS sumoku_solver)

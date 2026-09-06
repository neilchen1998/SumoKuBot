add_library(sumoku_solver sumokubacktrackingsolver.cc sumokubitmaskorderingsolver.cc sumokubitmasksolver.cc sumokumrvsolver.cc sumokuorderingsolver.cc)

target_include_directories(sumoku_solver PUBLIC ${PROJECT_SOURCE_DIR}/modules)

target_link_libraries(sumoku_solver PUBLIC board_library)

add_library(sumoku::solver ALIAS sumoku_solver)

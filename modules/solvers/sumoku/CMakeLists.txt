add_library(sumoku_solver
    sumokubacktrackingsolver.cc
    sumokubitmaskorderingsolver.cc
    sumokubitmasksolver.cc
    sumokumrvsolver.cc
    sumokuorderingsolver.cc
)

target_include_directories(sumoku_solver PUBLIC ${PROJECT_SOURCE_DIR}/modules)

target_link_libraries(sumoku_solver PRIVATE board_library spdlog::spdlog)
target_link_libraries(sumoku_solver PRIVATE project_options)

add_library(sumoku::solver ALIAS sumoku_solver)

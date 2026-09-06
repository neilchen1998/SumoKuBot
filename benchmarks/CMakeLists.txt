FetchContent_Declare(
  nanobench
  GIT_REPOSITORY https://github.com/martinus/nanobench.git
  GIT_TAG v4.3.10
)
FetchContent_MakeAvailable(nanobench)

# Create a CMake target
add_executable(mathbenchmark mathbenchmark.cc)
add_executable(solverbenchmark solverbenchmark.cc)

target_include_directories(mathbenchmark PRIVATE ${PROJECT_SOURCE_DIR}/modules)

# Create a variable for all the targets
set(MATH_TARGETS
  boost_math_library
)
set(SOLVER_TARGETS
  sudoku::solver
  sumoku::solver
  killer_sudoku::solver
)

# Link required libraries
# NOTE: ankerl::nanobench::templates::csv() requires fmt library
target_link_libraries(mathbenchmark PRIVATE
  nanobench
  fmt::fmt
  board_library
  ${MATH_TARGETS}
)
target_link_libraries(mathbenchmark PRIVATE project_options)
target_link_libraries(solverbenchmark PRIVATE
  fmt::fmt
  loader_library
  nanobench
  ${SOLVER_TARGETS}
)
target_link_libraries(solverbenchmark PRIVATE project_options)

# A custom target that runs all benchmarks
add_custom_target(benchmarks
  COMMAND $<TARGET_FILE:mathbenchmark>
  COMMAND $<TARGET_FILE:solverbenchmark>
  DEPENDS mathbenchmark solverbenchmark
  COMMENT "Running all benchmarks"
)

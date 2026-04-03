FetchContent_Declare(
  nanobench
  GIT_REPOSITORY https://github.com/martinus/nanobench.git
  GIT_TAG v4.3.10)
FetchContent_MakeAvailable(nanobench)

add_executable(mathbenchmark mathbenchmark.cc)
add_executable(solverbenchmark solverbenchmark.cc)

# Link required libraries
# NOTE: ankerl::nanobench::templates::csv() requires fmt library
target_link_libraries(mathbenchmark PRIVATE nanobench board_library math_library fmt::fmt)
target_link_libraries(solverbenchmark PRIVATE nanobench fmt::fmt solver_library loader_library)

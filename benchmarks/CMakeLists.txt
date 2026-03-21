FetchContent_Declare(
  nanobench
  GIT_REPOSITORY https://github.com/martinus/nanobench.git
  GIT_TAG v4.3.10)
FetchContent_MakeAvailable(nanobench)

# add_executable(boardbenchmark boardbenchmark.cc)
add_executable(solverbenchmark solverbenchmark.cc)


# Link required libraries
# NOTE: ankerl::nanobench::templates::csv() requires fmt library
# target_link_libraries(boardbenchmark PRIVATE nanobench board_library fmt::fmt)
target_link_libraries(solverbenchmark PRIVATE nanobench solver_library fmt::fmt)

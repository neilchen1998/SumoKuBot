# Sumoku

## Summary

This project solves [Sumoku](https://en.wikipedia.org/wiki/Killer_sudoku) (Killer Sudoku) problems with various approaches, such as:

- traditional approach (traversing from the left to the right and from the top to the bottom),
- ordering approach (exploring the box that contains least amount of elements)
- MRV (exploring the cell that has the minimum remaining values)

## Rules

Sumoku is a variation of Sudoku.

The objective is to fill the board with numbers from 1 to 9 and follow the following conditions:

- A number is unique in each row
- A number is unique in each column
- A number is unique in each box
- The sum of all numbers in a box should equal to the number provided

## Background Story

The lady sitting next to me on my flight back home was solving [Sudoku](https://en.wikipedia.org/wiki/Sudoku) problems. I chatted with her saying that I am also a fan of solving Sudoku problems and I have developed my own approach to crack this game. She then tore a page from her Sudoku maganize and handed me. It was Sumoku, a variation of Sudoku. I was flabbergasted when I first saw the puzzle as there are no hints but only sums to begin with. The lady and I chatted for a bit and I mentioned that I am a software engineer. When I disembarked the plane, I thought to myself, "Hmm..., what if I write a piece of software that solves Sumoku? This is very similar to Sudoku and I did a [LeetCode question](https://leetcode.com/problems/sudoku-solver/description/) like this before." And the rest is history.

## Requirements

The requirements are:

- CMake 3.18 or better; 4.0+ highly recommended
- A C++20 compatible compiler ([gcc](https://gcc.gnu.org/) or [llvm](https://llvm.org/))
- The Boost libararies
- Git
- Doxygen (optional, highly recommended)
- [fmt](https://github.com/fmtlib/fmt) 11.0 or higher (will automatically install if not present)
- [Catch2](https://github.com/catchorg/Catch2) 3.8 or higher (will automatically install if not present)
- [nanobench](https://github.com/martinus/nanobench.git) 4.3 or higher (will automatically install if not present)
- [abseil](https://github.com/abseil/abseil-cpp.git) 20250512.1 or newer (will automatically install if not present)

## Instructions

To configure:

```bash
cmake -S . -B build
```

Add `--toolchain=./<your_toolchain_file>.toolchain` if you want to use your own toolchain.

Add `-GNinja` if you have Ninja.

To build without example:

```bash
cmake --build build
```

To test (`--target` can be written as `-t` in CMake 3.15+):

```bash
cmake --build build --target test
```

To run the binary with example layout:

```bash
./build/apps/app
```

To build and test:

```bash
cmake --build build -DCMAKE_BUILD_TYPE=Test && cmake --build build --target test
```

To build docs (requires Doxygen, output in `build/docs/html`):

```bash
cmake --build build --target docs
```

To build and run benchmark:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Benchmark && ./build/bench/<name_of_benchmark>
```

## Notes

### Backtracking

Backtracking is a technique that finds solutions for constraint satisfaction problems. A constraint satisfaction problem

### Pruning

### Replace checking with bitmasks

Traversing rows and columns to verify that a number is unique is a *O(N)* operation.
In our 9-by-9 grid, it is still manageable but we can reduce it from *O(N)* to *O(1)* by using a bitmask.
Since we assume the board size (*N*) is 9 in the standard Sumoku game, we can use *uint16_t* to represent what numbers are present.
The 1st bit in the mask represents if the number *1* is present or not, the 2nd bit in the mask represents if the number *2* is present or not, etc.

Let say we are placing number *7* in a given row, we just need to do the following steps:

1. Bit-shift *1* by 7 times
2. Find the bitmask that presents the given row
3. OR the result from step (1) with the bitmask from step (2)

Let say we need to check if number *5* is already present in a given column, we just need to do the following steps:

1. Bit-shift *1* by 5 times
2. Find the bitmask that present the given column
3. AND the result from step (1) with the bitmask from step (2)

Overall, we need to use two vector of *uint16_t*'s (one for all the rows and another for all the columns) to represent the board.

The chart below shows the improvement from using bitmasks. We can see at least a 120% improvement on different approaches.

|               ms/op |                op/s |    err% |     total | Sumoku Solver Comparison #1
|--------------------:|--------------------:|--------:|----------:|:----------------------------
|              152.02 |                6.58 |    0.4% |      1.69 | `traditional`
|              112.37 |                8.90 |    0.2% |      1.24 | `traditional w/ bitmasks`
|               13.08 |               76.42 |    0.2% |      0.15 | `ordering`
|               11.04 |               90.56 |    0.2% |      0.12 | `ordering w/ bitmasks`

|               ms/op |                op/s |    err% |     total | Sumoku Solver Comparison #2
|--------------------:|--------------------:|--------:|----------:|:----------------------------
|               18.57 |               53.85 |    0.2% |      0.21 | `traditional`
|               13.95 |               71.67 |    0.2% |      0.15 | `traditional with bitmasks`
|              404.15 |                2.47 |    0.1% |      4.45 | `ordering`
|              334.99 |                2.99 |    0.2% |      3.68 | `ordering w/ bitmasks`

### Minimum Remaining Value (MRV) Heuristic

Minimum remaining value (MRV) heuristic is a way prioritize the selection of elements in a search algorithm.
We mentioned earlier that backtracking is essentially a depth-first search (DFS) algorithm, and that it searches through all potential solution spaces.
We can incorporate MRV heuristic into the search algorithm such that it searches potential solution spaces that have the least amount of possibilities (most constraints).

There are three constraints in Sumoku:

1. a number needs to be unique in each row,
2. a number needs to be unique in each column, and
3. the sum of all elements within a box should equal to the sum that is provided

We iterate all blank elements in each iteration and find the element with the minimum remaining value by calling *FindNextBestCell*.

```cpp
inline Selection FindNextBestCell()
{
    Selection ret;
    size_t curMinCnt = _N + 1;

    // Loop through the entire board to find the next best cell
    for (size_t r = 0; r < _N; ++r)
    {
        for (size_t c = 0; c < _N; ++c)
        {
            // Only check the cell that is empty
            if (_board[r][c] == 0)
            {
                // Get the candidates and the number of candidates
                uint16_t candidates = GetCandidates(r, c);

                // If there is no candidate available that means we hit a dead end and this tree needs to be pruned
                if (candidates == 0) [[unlikely]]
                {
                    return Selection {.deadEnd = true};
                }

                #ifdef __GNUC__
                int curNumOfCandidates = __builtin_popcount(candidates);
                #else
                int curNumOfCandidates = std::popcount(candidates);
                #endif

                // Update the return value when the current number of candidates is smaller than the previous one
                if (curNumOfCandidates < curMinCnt)
                {
                    curMinCnt = curNumOfCandidates;
                    ret.r = r;
                    ret.c = c;
                    ret.mask = candidates;

                    // If there is only one candidate then we return the current value early
                    if (curNumOfCandidates == 1)
                    {
                        return ret;
                    }
                }
            }
        }
    }

    return ret;
}
```

The function finds the element and return to *Backtrack*.

The following chart demonstrate the benefit of using MRV approach as it is faster than other solvers in average.
The drawback is that this algorithm is required to loop through all remaining elements at every iteration and can be slow if the size of the board is small.

|               ms/op |                op/s |    err% |     total | Sumoku Solver Comparison #1
|--------------------:|--------------------:|--------:|----------:|:----------------------------
|              112.37 |                8.90 |    0.2% |      1.24 | `traditional w/ bitmasks`
|               11.04 |               90.56 |    0.2% |      0.12 | `ordering w/ bitmasks`
|               17.54 |               57.02 |    0.9% |      0.19 | `MRV`

|               ms/op |                op/s |    err% |     total | Sumoku Solver Comparison #2
|--------------------:|--------------------:|--------:|----------:|:----------------------------
|               13.95 |               71.67 |    0.2% |      0.15 | `traditional with bitmasks`
|              334.99 |                2.99 |    0.2% |      3.68 | `ordering w/ bitmasks`
|               17.10 |               58.48 |    0.4% |      0.19 | `MRV`

### __builtin_popcount

*__builtin_popcount* is a GCC special function that returns the number of 1's (set bits) in a given unsigned int.
It leverages hardware instructions if possible, which is significantly faster than software implementation.
In C++20, one can use *std::popcount* if the compiler is not GCC compatible.
We use a preprocessor to check if the compiler is GCC and call different function accordingly.

```cpp
#ifdef __GNUC__
int curNumOfCandidates = __builtin_popcount(candidates);
#else
int curNumOfCandidates = std::popcount(candidates);
#endif
```

### Compiler flags

*target_compile_options* is available in modern **CMake**.
We create an interface library that only contains compiler flags and linker flags.

```cmake
# Compiler flags
add_library(project_options INTERFACE)

# Flags for all build types
target_compile_options(project_options INTERFACE -Wall -Wextra -Wpedantic)

# Flags based on the specified build type
target_compile_options(project_options INTERFACE
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Benchmark>:-O3 -march=native -DNDEBUG>
    $<$<CONFIG:Test>:-g -O1 -fsanitize=address -fsanitize=undefined>
)

# Linker flags
target_link_options(project_options INTERFACE
    $<$<CONFIG:Test>:-fsanitize=address -fsanitize=undefined>
)
```

We can then add *project_options* in *src/CMakeLists.txt*, *tests/CMakeLists.txt*, etc, like this so that the source code inside those subdirectories will be compiled with the flags defined in the main *CMakeLists.txt*:

```cmake
target_link_libraries(board_library INTERFACE  Boost::boost fmt::fmt project_options)
```

- `$<$<CONFIG:Release>:-O3`: adds *-O3* flag if the build type is set to *Release*
- *-march=native*: uses all available instructions on the machine
- *-DNDEBUG*: disable all standard *assert*s

### Variadic templates w/ concepts

Variadic templte was introduced back in **C++11** and can be paired with *concepts* in **C++20**.

We want to create a function that takes multiple digits and output the mask that contains them.
The number of digits varies therefore creatin a variadic function is the best option.
Also, we would only take argument that is of type **int**.
Hence we add *requires* in the template.

```cpp
template <typename... Args>
requires (std::same_as<Args, int>&&...)
uint16_t GenerateCandidateMask(Args... digit)
{
    return ((1U << digit) | ... | 0);
}
```

Variadic functions could be gnarly at first glance.
But we can easily break it down and see what it does.

We can expand `return ((1U << digit) | ... | 0);` to `return ((1U << digit1) | (1U << digit2) | (1U << digit3)| 0);`

And for the *requires* part, we need to add *&&* there since it represents the AND operator after expansion:

`(std::same_as<Args1, int> && std::same_as<Args2, int> && std::same_as<Args3, int>)`.

## Reference

- [gprof2dot](https://pypi.org/project/gprof2dot/)
- [Visually Profile C++ Program Performance](https://www.youtube.com/watch?v=zbTtVW64R_I)

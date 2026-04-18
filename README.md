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

Killer Sudoku is a variation of Sudoku.

Killer Sudoku has small 3-by-3 grids like a regular Sudoku with rules like Sumoku.

Note: In this repo, there are two types of Sumoku, the one that does not have 3-by-3 grids, will be referred to **Sudoku** and the one that has 3-by-3 grids will be referred to **Killer Sudoku**.

## Background Story

The lady sitting next to me on my flight back home was solving [Sudoku](https://en.wikipedia.org/wiki/Sudoku) problems. I chatted with her saying that I am also a fan of solving Sudoku problems. She tore a page from her Sudoku magazine and handed me. It was not standard Sudoku, but Sumoku-a variation of Sudoku that I have never seen before. I was flabbergasted as it is provided with no hints but only sums to begin with. I gave up solving the puzzle on my flight and started to chat with the lady. I mentioned that I am a software engineer to her. She believes that I am smart enough to solve this Sumoku. When I disembarked the plane, I thought to myself, "Hmm..., what if I write a piece of software that solves Sumoku? This is very similar to Sudoku and I did a [LeetCode question](https://leetcode.com/problems/sudoku-solver/description/) like this before." And the rest is history.

## Requirements

The requirements are:

- CMake 3.21 or better; 4.0+ highly recommended
- A C++23 compatible compiler ([gcc](https://gcc.gnu.org/) or [llvm](https://llvm.org/))
- [Git](https://git-scm.com/)
- [abseil](https://github.com/abseil/abseil-cpp.git) 20250512.1 or newer (will automatically install if not present)
- [Boost](https://www.boost.org/) 1.67 or higher (will automatically install if not present)
- [Catch2](https://github.com/catchorg/Catch2) 3.8 or higher (will automatically install if not present)
- [CLI11](https://github.com/CLIUtils/CLI11.git) v2.4.1 or higher (will automatically install if not present)
- [fmt](https://github.com/fmtlib/fmt) 11.0 or higher (will automatically install if not present)
- [json](https://github.com/nlohmann/json.git) 3.0.0 or higher (will automatically install if not present)
- [nanobench](https://github.com/martinus/nanobench.git) 4.3 or higher (will automatically install if not present)
- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) (optional, highly recommended)
- [Doxygen](https://www.doxygen.nl/) (optional, highly recommended)

## Instructions

To generate the build system:

```zsh
cmake -S . -B build
```

To generate the build system with unit tests enabled (**tests** subdirectory):

```zsh
cmake -S . -B build -DPROJECT_BUILD_TESTS=ON
```

To generate the build system with benchmarks enabled (**benchmarks** subdirectory):

```zsh
cmake -S . -B build -DPROJECT_BUILD_BENCHMARKS=ON
```

For 64-bit Linux systems:

```zsh
cmake -S . -B build --toolchain=./linux-x86_64.toolchain
```

Add `--toolchain=./<your_toolchain_file>.toolchain` if you want to use your own toolchain.

Add `-GNinja` if you want to use Ninja.

To build with release configuration:

```zsh
cmake --build build --config Release
```

To build with debug configuration:

```zsh
cmake --build build --config Debug
```

To test (`--target` can be written as `-t` in CMake 3.15+):

```zsh
cmake --build build --target test
```

To run the binary with example layout:

```zsh
./build/apps/app
```

To build and test:

```zsh
cmake --build build -DCMAKE_BUILD_TYPE=Test && cmake --build build --target test
```

Run a specific tag:

```zsh
./build/tests/solvertestlib "[<tag>]"
```

To build docs (requires Doxygen, output in `build/docs/html`):

```zsh
cmake --build build --target docs
```

To build and run benchmark:

```zsh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Benchmark && ./build/bench/<name_of_benchmark>
```

To build and generate **compile_commands.json** (which clang-tidy relies on):

```zsh
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

To run clang-tidy:

```zsh
run-clang-tidy -p build/ '.*/(apps|benchmarks|tests)/.*'
```

## Benchmark

The folllowing table shows the latest iteration of the MRV method versus the traditional method (the very first iteration).
We observed a 25x performance increase, though results fluctuate based on the specific puzzle.

|               ns/op |                op/s |    err% |     total | Sumoku Solver Comparison #1
|--------------------:|--------------------:|--------:|----------:|:----------------------------
|            1,435.72 |          696,516.87 |    1.1% |      0.01 | `traditional`
|              418.31 |        2,390,574.08 |    1.0% |      0.01 | `MRV`

|               ns/op |                op/s |    err% |     total | Sumoku Solver Comparison #2
|--------------------:|--------------------:|--------:|----------:|:----------------------------
|            2,968.19 |          336,905.81 |    0.9% |      0.01 | `traditional`
|              783.57 |        1,276,207.05 |    1.5% |      0.01 | `MRV`

|               ns/op |                op/s |    err% |     total | Sumoku Solver Comparison #3
|--------------------:|--------------------:|--------:|----------:|:----------------------------
|       15,122,833.00 |               66.13 |    0.2% |      0.17 | `traditional`
|          881,625.00 |            1,134.27 |    2.4% |      0.01 | `MRV`

|               ns/op |                op/s |    err% |     total | Sumoku Solver Comparison #4
|--------------------:|--------------------:|--------:|----------:|:----------------------------
|        2,000,916.00 |              499.77 |    3.3% |      0.02 | `traditional`
|           88,583.33 |           11,288.81 |    1.2% |      0.01 | `MRV

The following table shows the latest iteration of the MRV metion solving 3 Killer Sudoku puzzles.
The [last puzzle](https://www.calcudoku.org/hardest_logic_number_puzzles/) is considered to be the hardest Killer Sudoku puzzle ever exists.

|               ms/op |                op/s |    err% |     total | Killer Sudoku
|--------------------:|--------------------:|--------:|----------:|:--------------
|                0.03 |           33,895.50 |    1.5% |      0.01 | `MRV - #1`
|                0.22 |            4,521.31 |    1.5% |      0.01 | `MRV - #2`
|               56.66 |               17.65 |    0.3% |      0.62 | `MRV - #3`
|                0.07 |           14,942.71 |    0.6% |      0.01 | `MRV - #4`

Note: The above results are generated by building it with **-O3 -march=native -flto -ffast-math -DNDEBUG** on a Mac Mini with M4 chip with 24 GB of RAM.

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

### Minimum remaining value (MRV) heuristic

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
    size_t curMinCnt = N_ + 1;

    // Loop through the entire board to find the next best cell
    for (size_t r = 0; r < N_; ++r)
    {
        for (size_t c = 0; c < N_; ++c)
        {
            // Only check the cell that is empty
            if (board_[r][c] == 0)
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

### __builtin_ctz

*__builtin_ctz* (count trailing zeros) is a GCC special function that returns the number of consecutive 0's from the right (least significant bit).
It leverages hardware instructions if possible, which is significantly faster than software implementation.
In C++20, one can use *std::countr_zero* if the compiler is not GCC compatible.
If the input is 41 (**0b101001**) then the return value will be 0 and if the input is 48 (**0b110000**), then the return value will be 4.

### Pre-calculate candidates

We know that there can be at most 9 elements in a box and the maximum sum is 45 ($\sum_{i=1}^{9} i$).
Therefore, we can pre-calculate all the candidates for a given box in the compile-time.
We use **dynamic programming** to achieve this, which we will cover that in the next section.
For now, we create an API called *GetPossibleNumbersMask* that so that we can get all the candidates in the mask format in the beginning

```cpp
constexpr uint16_t GetPossibleNumbersMask(size_t target, size_t count)
{
    static constexpr PossibleNumbersTable table{};
    return table.get(target, count);
}
```

Now we can use *GetPossibleNumbersMask* to improve our *FindNextBestCell()*.

```cpp
uint16_t GetCandidates(size_t r, size_t c)
{
    size_t id = boxID_[r][c];

    uint16_t forbidden = rowMask_[r] | colMask_[c] | boxMask_[id];
    uint16_t ret = 0U;

    for (size_t v = 1; v <= N_; ++v)
    {
        // Check if the current number is possible
        if (!(forbidden & (1U << v)) && (_options[r][c] >> v))
        {
            ret |= (1U << v);
        }
    }

    return ret;
}
```

The following table shows the improvement by using pre-calculated candidate approach.
We see a whopping 120 times improvement over the old method.

|               ns/op |                op/s |    err% |     total | Sumoku Solver Comparison #1
|--------------------:|--------------------:|--------:|----------:|:----------------------------
|            1,568.39 |          637,595.16 |    0.8% |      0.01 | `traditional`
|              518.23 |        1,929,629.18 |    4.0% |      0.01 | `MRV`

|               ns/op |                op/s |    err% |     total | Sumoku Solver Comparison #2
|--------------------:|--------------------:|--------:|----------:|:----------------------------
|            2,842.32 |          351,825.79 |    1.3% |      0.01 | `traditional`
|              820.51 |        1,218,759.94 |    1.3% |      0.01 | `MRV`

|               ns/op |                op/s |    err% |     total | Sumoku Solver Comparison #3
|--------------------:|--------------------:|--------:|----------:|:----------------------------
|       14,526,208.00 |               68.84 |    0.6% |      0.16 | `traditional`
|          717,833.00 |            1,393.08 |    1.2% |      0.01 | `MRV`

|               ns/op |                op/s |    err% |     total | Sumoku Solver Comparison #4
|--------------------:|--------------------:|--------:|----------:|:----------------------------
|        1,874,208.00 |              533.56 |    2.7% |      0.02 | `traditional`
|           71,750.00 |           13,937.28 |    1.2% |      0.01 | `MRV`

### Find candidates that can sum up to a target

In a typical Sumoku game, the valid number goes from 1 to 9. Therefore the number of combinaitons is $2^9$.
The search space is not too large, therefore we can loop through all possible combinations to precalculate all the candidates that can sum up to a target.

We use **i** to represents the current combination of numbers. Since the number 0 is not a valid candidate, we just ignore it and let the last bit of **i** to represent the number 1, the second last bit of **i** to represent the number 2, etc.

However, since the mask is 0-based, i.e., the last digit represents the number 0, the second last bit represents the number 1, we need to convert **i** to **mask** before we can put it to **table**.

```text
0b001: 1
0b010: 2
0b100: 3
```

We can get the number that **i** represents by adding the number of trailing 0's (*__builtin_ctz*: count trailing zeros) and 1.


And since each 1 bit in **i** represents a digit, we can get the current number of digits by counting the numbers of 1's in **i** by using *__builtin_popcount*.

```cpp
for (uint16_t i = 0U; i < (1U << MAX_COUNT); ++i)
{
    // Get the count of the numbers
    int cnt = 0;
    #ifdef __GNUC__
    cnt = __builtin_popcount(i);
    #else
    cnt = std::popcount(i);
    #endif

    uint16_t mask = i;
    uint16_t sum = 0;

    uint16_t tmp = i;
    while (tmp)
    {
        // Convert to number and add to the current sum
        #ifdef __GNUC__
        sum += __builtin_ctz(tmp) + 1;
        #else
        sum += std::countr_zero(tmp) + 1;
        #endif

        tmp &= (tmp - 1);   // removes the rightmost 1 bit
    }

    // Store the current count and the sum into the table
    table[cnt][sum] |= (mask << 1);
}
```

This new version of finding all candidates that can sum up to a target is easier and more straight-forward compared to the dynamic programming that we will cover later.

### Dynamic programming

Dynamic programming is a very powerful algorithm that breaks down complex problems into smaller sub-problems.
There are multiple variations of dynamic programming, e.g., recursion, linear, or 2D.
In our case, we need to create a 3D version.

This is probably the most challenging part in this repository.
A 2D dynamic programming method was used but failed test cases and ChatGPT came to the rescue.
(Yes, I am not ashamed that I used ChatGPT for help.)

We know that we need to find all the numbers that can sum up to a target in compile-time so that we can skip the calculations at run-time.
The state space is { the number of digits, the target sum }.
The value of each element represents the combinations of candidates.
There are many combinations and can not be consolidated into one single mask (I made that mistake in the beginning).

The core logic is that how to find the combinations of candidates that can sum up to 22 with three digits that includes the digit 5.
We know that if this statement is true if and only if we can sum up to 17 ($22 - 5 = 17$) with two digits.

For each element in the 2D dynamic programming grid, we need an additional dimension that stores all the possible combinations.

`states[i][j][k]` means that the kth combination that can sum up to *j* by using *i* digits.
The third dimension is essential since it represents how a specific combination of digits is constructed and two combinations of digits can not be mixed. Meaning that ${2, 3}$ and ${1, 4}$ can both sum up to 5 but does not mean that we can use the first combination to sum up to 8 with the digit 3 since 3 is already present.

```cpp
struct PossibleNumbersTable
{
    static constexpr int MAX_SUM   = 45;
    static constexpr int MAX_COUNT = 9;

    std::array<std::array<uint16_t, MAX_SUM + 1>, MAX_COUNT + 1> table {};

    consteval PossibleNumbersTable()
    {
        struct State
        {
            /// @brief All the masks (combinations)
            // For instance, masks[1] with a value of 1b110 means {1, 2} is a valid combination of candidates
            uint16_t masks[1 << MAX_COUNT] {};  // there are 2^MAX_COUNT possibilities (in this case 2^9)

            /// @brief The number of combinations (also used as an index)
            size_t sz = 0;
        };

        std::array<std::array<State, MAX_SUM + 1>, MAX_COUNT + 1> states {};

        // The base case
        states[0][0].masks[states[0][0].sz++] = 0;

        // Iterate from number 1 to 9
        for (int digit = 1; digit <= MAX_COUNT; ++digit)
        {
            // Iterate the number of digits and target in reverse to ensure each digit is used only once
            // NOTE: Iterating forward from k = 1 to k = 9 would allow the current digit to be added
            // to a sum that already includes it, leading duplicated digits.
            // Reversing the loops ensures we only build results from the previous iteration
            for (int c = MAX_COUNT; c >= 1; --c)
            {
                for (int s = MAX_SUM; s >= digit; --s)
                {
                    // Get the previous state
                    // The previous state has one less number of digits, therefore it's (c - 1)
                    // and it has less sum, i.e., (s - num)
                    State& prev = states[c - 1][s - digit];
                    State& curr = states[c][s];

                    // Propagate all candidates from previous state to the current state and
                    // add the current digit to the mask
                    for (int i = 0; i < prev.sz; ++i)
                    {
                        curr.masks[curr.sz++] = prev.masks[i] | (1u << digit);
                    }
                }
            }
        }

        // Construct the final table that combine masks from all states
        for (int c = 0; c <= MAX_COUNT; ++c)
        {
            for (int s = 0; s <= MAX_SUM; ++s)
            {
                uint16_t mask = 0;
                const State& st = states[c][s];

                // Loop through all masks
                for (int i = 0; i < st.sz; ++i)
                {
                    mask |= st.masks[i];
                }

                table[c][s] = mask;
            }
        }
    }

    /// @brief Finds all the candidates of k distinct digits that sum up to a given target
    /// @param target The target sum
    /// @param k The number of distinct digit(s)
    /// @return All the candidates in mask format
    [[nodiscard]] constexpr uint16_t get(size_t target, size_t count) const
    {
        if (count > MAX_COUNT || target > MAX_SUM)  return 0;

        return table[count][target];
    }
};
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

### Module

*%* is a very computationally expensive operator as it can take up to multiple CPU cycles.
Whereas, *&* only requires a single CPU instruction.

In benchmark functions when we need to iterate an array with multiple elements in it, we need to make sure the index stays within bound.
One might use ```(i++) % sz```.
But it can be optimized by using:

```cpp
(i++) & (sz - 1)
```

, when the size of the array is a power of 2.

This is a very common low-level programming techique that optimizes the code.
In our case, if the size of the array is 1024 (which is $2^{10}$), and the current index is $1025$,
we need to access the first element of the array.

We only care about the last 7 bits and by subtracting one from 1024, we are effectively getting the last 7 bits.

This is the math behind it:
```text
  0b10000000001
& 0b01111111111
---------------
  0b00000000001
```

### Serialize and Deserialize Data

**nlohmann/json** is the golden tool to serialize and deserialize data for C++ projects.
It is a header-only and light-weight tool that is easy to use.

In order for nlohmann::json to know the data structure, we need to provide the arguments of a structure by calling *NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE*.
*NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE* is a macro and the first argument is the name of the structure, followed by the member elements.

```cpp
#include <nlohmann/json.hpp>

struct Point
{
    size_t x;
    size_t y;

    // Use the default == operator
    bool operator==(const Point&) const = default;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Point, x, y) // for nlohmann::json

/// @brief The Sumoku test data structure
struct SumokuPuzzleData
{
    size_t N;
    std::vector<std::vector<Point>> boxes;
    std::vector<int> sums;
    std::string label;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SumokuPuzzleData, N, boxes, sums, label)   // for nlohmann::json
```

### Data-driven testing (DDT)

Data-driven testing is a technique that uses external data and import it into the test environment.
This approach eliminates the need to incorportate test cases into the test source code.
One can just create new tests in the test directory and rerun the test binary.

We can create a Sumoku test case by running this main function:

```cpp
int main()
{
    SumokuPuzzleData myPuzzle
    {
        9,
        {
            {{0, 0}, {0, 1}}, {{0, 2}, {1, 2}}, {{0, 3}, {0, 4}}, {{0, 5}, {1, 5}}, {{0, 6}, {0, 7}}, {{0, 8}, {1, 8}},
            {{1, 0}, {2, 0}}, {{1, 1}, {2, 1}}, {{1, 3}, {1, 4}}, {{1, 6}, {1, 7}},
            {{2, 2}, {2, 3}}, {{2, 4}, {3, 4}}, {{2, 5}, {2, 6}}, {{2, 7}, {2, 8}},

            {{3, 0}, {4, 0}}, {{3, 1}, {3, 2}}, {{3, 3}, {4, 3}}, {{3, 5}, {3, 6}}, {{3, 7}, {3, 8}},
            {{4, 1}, {4, 2}}, {{4, 4}, {5, 4}}, {{4, 5}, {5, 5}}, {{4, 6}, {4, 7}}, {{4, 8}, {5, 8}},
            {{5, 0}, {5, 1}}, {{5, 2}, {5, 3}}, {{5, 6}, {5, 7}},

            {{6, 0}, {6, 1}}, {{6, 2}, {7, 2}}, {{6, 3}, {6, 4}}, {{6, 5}, {7, 5}}, {{6, 6}, {7, 6}}, {{6, 7}, {6, 8}},
            {{7, 0}, {7, 1}}, {{7, 3}, {7, 4}}, {{7, 7}, {7, 8}},
            {{8, 0}, {8, 1}, {8, 2}}, {{8, 3}, {8, 4}}, {{8, 5}, {8, 6}}, {{8, 7}, {8, 8}}
        },
        {
            8, 10, 13, 7, 11, 14,
            11, 8, 9, 10,
            12, 15, 6, 13,

            5, 11, 10, 9, 7,
            14, 11, 6, 8, 12,
            10, 9, 13,

            7, 11, 8, 10, 14, 9,
            12, 13, 5,
            15, 10, 7, 11
        },
        "P7"
    };

    nlohmann::json j(myPuzzle);

    std::ofstream file("./tests/data/puzzle_p8.json");

    if (file.is_open())
    {
        file << j.dump(4);
        file.close();
    }

    return 0;
}
```

**nlohmann::json** sees the macros defined in *board/boardlib.hpp* for both **SumokuPuzzleData** and **Point**.
Therefore it can handle the data structure for us.

We then use this function to load the data (test cases) into the test.
Again, since we have the macros for both **SumokuPuzzleData** and **Point**, *nlohmann::json* can deserialize the data.

```cpp
std::vector<SumokuPuzzleData> LoadAllPuzzles(std::string_view dir)
{
    std::vector<SumokuPuzzleData> testCases;

    // Iterate over all the json entries in the directory
    for (const auto& entry : fs::directory_iterator(dir))
    {
        if (entry.path().extension() == ".json")
        {
            std::ifstream file{entry.path()};
            nlohmann::json j;
            file >> j;

            SumokuPuzzleData puzzle = j.get<SumokuPuzzleData>();
            testCases.push_back(puzzle);
        }
    }

    return testCases;
}
```

We also need to change the **test section** in our **test case** since there are now multiple test cases.
Luckily, *Catch2* supports this method.
We can use *DYNAMIC_SECTION* and load the test cases by using *GENERATE* to generate multiple test cases for us.
We just need to provide an array or vector of test cases and *Catch2* and automatically does it for us.

```cpp
TEST_CASE("Sumoku (SumokuMRV) Suite", "[SumokuMRV]")
{
    // Load all the test cases
    static std::string folder = GetTestDataPath();
    static std::vector<SumokuPuzzleData> all_puzzles = LoadAllPuzzles(folder);

    // Check the vector to make sure it contains at least one test case
    REQUIRE_FALSE(all_puzzles.empty());

    const SumokuPuzzleData& data = GENERATE(from_range(all_puzzles));

    // The section
    DYNAMIC_SECTION("Puzzle: " << data.label)
    {
        solver::SumokuMRV s {data.N, data.boxes, data.sums};

        s.Solve();

        auto ret = s.GetSolution();
        REQUIRE (ret != std::nullopt);

        std::vector<std::vector<size_t>> solution = *ret;

        REQUIRE (solution.size() == data.N);
        validate_boad_is_square(solution);
        validate_sukodu_row_column_constraints(solution);
    }
}
```

Note that we want to see the name of the test case should a test case fails, therefore we need to provide the label or the name of a test to the title.
This eliminate the need to write multiple sections and the code inside the section can be reused.

### Assignment vs. constructor vs. list-initialization

Assignment is used with an equal sign (*=*) before the new value.

We can assign a vector to anothe vector like this:

```
std::vector<int> a, b;
a = b;
```

Now the vector *a* will copy the values from vector *b*.

Constructor is used with a pair of parentheses *()* that can construct the object from the get-go.

We can construct a vector like this:

```
std::vector<int> vec({1, 2, 3, 4});
```

List-initialization is used with a pair of braces (*{}*) with the elements inside it.

We can construct a vector like this:

```
std::vector<int> vec{1, 2, 3, 4};
```

In this case, the vector contains all the elements inside the braces.

We need to beware of the differences between a constructor and a list-initilization since the result of *nlohmann::json j(myPuzzle);* is not the same as *nlohmann::json j{myPuzzle};*.

The first line of code is saying that I want to construct an instance of *nlohmann::json* that takes an instance of *myPuzzle*,
whereas the second line of code is saying that I want to construct a list of *nlohmann::json*'s that has one instance of *myPuzzle*.
The output of the JSON files will be different as well.

### `std::mdspan`

`std::mdspan` was introduced in C++23 as a non-owning , multi-dimensional view over a contiguous sequence of elements.
Packing elements in a contiguous block in memory improve cache efficiency.
By using `std::mdspan`, the user can just access the element by using $(i, j, k)$ without the need to calculate the exact index of the element in its array.

Here is a way to create a view of a 2D vector.
We first needs to create the underlying containter, in this case we are using a `std::vector`.
Then we need to declare and create the view of the container with the dimension.
In this case, it is a 2D view, with N rows of M integers.
We then can access the element by the row-column index of an element.

```cpp
#include <std::mdspan>  // std::mdspan, std::dextents

int main()
{
    const int N = 3;
    const int M = 4;
    std::vector<int> board();
    std::mdspan<int, std::dextents<size_t, 2>> boardView(board.data(), N, M);

    fmt::println("Element @ (1, 2): {}", boardView[1, 2]);
}
```

## Reference

- [gprof2dot](https://pypi.org/project/gprof2dot/)
- [Visually Profile C++ Program Performance](https://www.youtube.com/watch?v=zbTtVW64R_I)
- [Data-Driven Testing](https://www.leapwork.com/blog/a-short-introduction-to-data-driven-testing)
- [Killer Sudoku Puzzles](https://github.com/tommy-andersen/killer-sudoku-solver/blob/main/expert-1.json)
- [One of the World's Hardest Killer Sudokus](https://www.calcudoku.org/hardest_logic_number_puzzles/)

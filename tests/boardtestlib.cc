#define CATCH_CONFIG_MAIN

#include <array>    // std::array
#include <vector>    // std::vector
#include <unordered_set>    // std::unordered_set

#include <catch2/catch_all.hpp> // GENERATE
#include <catch2/catch_test_macros.hpp> // TEST_CASE, SECTION, REQUIRE

#include "board/boardlib.hpp"

constexpr int CountCombinationThreeNumbers(int target)
{
    int count = 0;
    for (size_t i = 1; i <= 9; ++i)
    {
        for (size_t j = i + 1; j <= 9; ++j)
        {
            for (size_t k = j + 1; k <= 9; ++k)
            {
                count += ((i + j + k) == target) ? 1 : 0;
            }
        }
    }

    return count;
}

constexpr int CountCombinationFourNumbers(int target)
{
    int count = 0;
    for (size_t i = 1; i <= 9; ++i)
    {
        for (size_t j = i + 1; j <= 9; ++j)
        {
            for (size_t k = j + 1; k <= 9; ++k)
            {
                for (size_t u = k + 1; u <= 9; ++u)
                {
                    count += ((i + j + k + u) == target) ? 1 : 0;
                }
            }
        }
    }

    return count;
}

TEST_CASE( "Unordered Map", "[main]" )
{
    std::unordered_map<Point, int, PointHasher> m;

    SECTION("Inserting and retrieving elements")
    {
        Point a {1, 2};
        Point b {3, 4};

        m[a] = 10;
        m.insert({b, 20});

        REQUIRE (m.size() == 2);
        REQUIRE (m[a] == 10);
        REQUIRE (m.at(b) == 20);
    }

    SECTION("Accessing non-existent keys")
    {
        REQUIRE_THROWS_AS (m.at({99, 98}), std::out_of_range);

        REQUIRE (m[{3, 5}] == 0);
        REQUIRE (m.size() == 1);
    }
}

TEST_CASE( "Combinations (Recursive)", "[main]" )
{
    constexpr std::array<int, 18> cache = []()
    {
        std::array<int, 18> arr {};
        for (size_t i = 0; i < 18; ++i)
        {
            arr[i] = CalCombinations(i, 2);
        }

        return arr;
    }();    // () invokes the lambda immediately

    SECTION("Lower section")
    {
        REQUIRE (cache[0] == 0);
        REQUIRE (cache[1] == 0);
        REQUIRE (cache[2] == 0);

        // Combinations: {1, 2}
        REQUIRE (cache[3] == 1);
    }

    SECTION("Middle section")
    {
        // Combinations: {1, 8}, {2, 7}, {3, 6}, {4, 5}
        REQUIRE (cache[9]  == 4);

        // Combinations: {1, 9}, {2, 8}, {3, 7}, {4, 6}
        REQUIRE (cache[10] == 4);

        // Combinations: {2, 9}, {3, 8}, {4, 7}, {5 ,6}
        REQUIRE (cache[11] == 4);
    }

    SECTION("Upper section")
    {
        // Combinations: {7, 9}
        REQUIRE (cache[16] == 1);

        // Combinations: {8, 9}
        REQUIRE (cache[17] == 1);
    }

    SECTION("No duplicates (a != b)")
    {
        // Combinations: {1, 9}, {2, 8}, {3, 7}, {4, 6}
        REQUIRE (cache[10] == 4);

        // Combinations: {1, 5}, {2, 4}
        REQUIRE (cache[6] == 2);
    }
}

TEST_CASE( "Combinations (Dynamic Programming)", "[main]" )
{
    SECTION("Two numbers sum up to a target")
    {
        constexpr std::array<int, 18> cache = []()
        {
            std::array<int, 18> arr {};
            for (size_t i = 0; i < 18; ++i)
            {
                arr[i] = CalCombinations(i, 2);
            }

            return arr;
        }();

        for (size_t i = 0; i < 18; ++i)
        {
            REQUIRE (CountCombinations(i, 2) == cache[i]);
        }
    }

    SECTION("Three numbers sum up to a target")
    {
        auto i = GENERATE(range(6, 25));

        REQUIRE (CountCombinations(i, 3) == CountCombinationThreeNumbers(i));
    }

    SECTION("Four numbers sum up to a target")
    {
        auto i = GENERATE(range(6, 30));

        int res = CountCombinations(i, 4);
        int ans = CountCombinationFourNumbers(i);

        // Print the values if the REQUIRE fails
        CAPTURE(i, res, ans);

        REQUIRE (res == ans);
    }
}

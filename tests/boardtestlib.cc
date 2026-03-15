#define CATCH_CONFIG_MAIN

#include <array>    // std::array
#include <vector>    // std::vector
#include <unordered_set>    // std::unordered_set

#include <catch2/catch_all.hpp> // GENERATE
#include <catch2/catch_test_macros.hpp> // TEST_CASE, SECTION, REQUIRE

#include "board/boardlib.hpp"

/// @brief Gets the number of candidate that can sum up to a given target with two numbers O(N^2)
/// @param target The target
/// @return The number of candidates
constexpr int CountCombinationTwoNumbers(int target)
{
    int count = 0;
    for (int i = 1; i <= 9; ++i)
    {
        for (int j = i + 1; j <= 9; ++j)
        {
            count += ((i + j) == target) ? 1 : 0;
        }
    }

    return count;
}

/// @brief Gets the number of candidate that can sum up to a given target with three numbers O(N^3)
/// @param target The target
/// @return The number of candidates
constexpr int CountCombinationThreeNumbers(int target)
{
    int count = 0;
    for (int i = 1; i <= 9; ++i)
    {
        for (int j = i + 1; j <= 9; ++j)
        {
            for (int k = j + 1; k <= 9; ++k)
            {
                count += ((i + j + k) == target) ? 1 : 0;
            }
        }
    }

    return count;
}

/// @brief Gets the number of candidate that can sum up to a given target with four numbers O(N^4)
/// @param target The target
/// @return The number of candidates
constexpr int CountCombinationFourNumbers(int target)
{
    int count = 0;
    for (int i = 1; i <= 9; ++i)
    {
        for (int j = i + 1; j <= 9; ++j)
        {
            for (int k = j + 1; k <= 9; ++k)
            {
                for (int u = k + 1; u <= 9; ++u)
                {
                    count += ((i + j + k + u) == target) ? 1 : 0;
                }
            }
        }
    }

    return count;
}

/// @brief Gets all the candidate that can sum up to a given target with two numbers O(N^2)
/// @param target The target
/// @return All the candidates in mask format
constexpr uint16_t GetCandiateMaskWithTwoNumbers(int target)
{
    uint16_t ret = 0U;
    for (int i = 1; i <= 9; ++i)
    {
        for (int j = i + 1; j <= 9; ++j)
        {
            if ((i + j) == target)
            {
                ret |= (1U << i);
                ret |= (1U << j);
            }
        }
    }

    return ret;
}

/// @brief Gets all the candidate that can sum up to a given target with three numbers O(N^3)
/// @param target The target
/// @return All the candidates in mask format
constexpr uint16_t GetCandidateMaskWithThreeNumbers(int target)
{
    uint16_t ret = 0U;
    for (int i = 1; i <= 9; ++i)
    {
        for (int j = i + 1; j <= 9; ++j)
        {
            for (int k = j + 1; k <= 9; ++k)
            {
                if ((i + j + k) == target)
                {
                    ret |= (1U << i);
                    ret |= (1U << j);
                    ret |= (1U << k);
                }
            }
        }
    }

    return ret;
}

/// @brief Generates the mask that contains all candidates
/// @tparam ...Args The type of the digit
/// @param ...digit The digit(s)
/// @return The candidates in mask format
template <typename... Args>
requires (std::same_as<Args, int>&&...)
uint16_t GenerateCandidateMask(Args... digit)
{
    return ((1U << digit) | ... | 0);
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

TEST_CASE( "Get Possible Numbers In Mask", "[main]" )
{
    SECTION("Lower section")
    {
        REQUIRE (GetPossibleNumbersMask(0, 2) == 0);
        REQUIRE (GetPossibleNumbersMask(1, 2) == 0);
        REQUIRE (GetPossibleNumbersMask(2, 2) == 0);

        // Combinations: {1, 2}
        REQUIRE (GetPossibleNumbersMask(3, 2) == GenerateCandidateMask(1, 2));
    }

    SECTION("Middle section")
    {
        // Combinations: {1, 8}, {2, 7}, {3, 6}, {4, 5}
        REQUIRE (GetPossibleNumbersMask(9, 2)  == GenerateCandidateMask(1, 8, 2, 7, 3, 6, 4, 5));

        // Combinations: {1, 9}, {2, 8}, {3, 7}, {4, 6}
        REQUIRE (GetPossibleNumbersMask(10, 2) == GenerateCandidateMask(1, 9, 2, 8, 3, 7, 4, 6));

        // Combinations: {2, 9}, {3, 8}, {4, 7}, {5 ,6}
        REQUIRE (GetPossibleNumbersMask(11, 2) == GenerateCandidateMask(2, 9, 3, 8, 4, 7, 5, 6));
    }

    SECTION("Upper section")
    {
        // Combinations: {7, 9}
        REQUIRE (GetPossibleNumbersMask(16, 2) == GenerateCandidateMask(7, 9));

        // Combinations: {8, 9}
        REQUIRE (GetPossibleNumbersMask(17, 2) == GenerateCandidateMask(8, 9));
    }

    SECTION("No duplicates (a != b)")
    {
        // Combinations: {1, 9}, {2, 8}, {3, 7}, {4, 6}
        REQUIRE (GetPossibleNumbersMask(10, 2) == GenerateCandidateMask(1, 9, 2, 8, 3, 7, 4, 6));

        // Combinations: {1, 5}, {2, 4}
        REQUIRE (GetPossibleNumbersMask(6, 2) == GenerateCandidateMask(1, 5, 2, 4));
    }

    SECTION("Two numbers sum up to a target")
    {
        auto target = GENERATE(range(3, 45));

        uint16_t res = GetPossibleNumbersMask(target, 2);
        uint16_t ans = GetCandiateMaskWithTwoNumbers(target);

        // Print the values if the REQUIRE fails
        CAPTURE(target, res, ans);

        REQUIRE (res == ans);
    }

    SECTION("Three numbers sum up to a target")
    {
        auto target = GENERATE(range(6, 45));

        uint16_t res = GetPossibleNumbersMask(target, 3);
        uint16_t ans = GetCandidateMaskWithThreeNumbers(target);

        // Print the values if the REQUIRE fails
        CAPTURE(target, res, ans);

        REQUIRE (res == ans);
    }
}

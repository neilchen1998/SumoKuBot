#define CATCH_CONFIG_MAIN

#include <vector>    // std::vector
#include <unordered_set>    // std::unordered_set

#include <catch2/catch_test_macros.hpp> // TEST_CASE, SECTION, REQUIRE
#include <catch2/matchers/catch_matchers_all.hpp>   // Catch::Matchers::Equals

#include "board/boardlib.hpp"

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

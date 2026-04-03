#define CATCH_CONFIG_MAIN

#include <unordered_set>    // std::unordered_set

#include <catch2/catch_all.hpp> // GENERATE
#include <catch2/catch_test_macros.hpp> // TEST_CASE, SECTION, REQUIRE, REQUIRE_THROWS_AS

#include "board/boardlib.hpp"   // Point
#include "math/mathlib.hpp" // PointHasher, BoostPointHasher

TEMPLATE_TEST_CASE("Unordered Map w/ Hashers", "[unordered_map][hasher]", PointHasher, BoostPointHasher)
{
    std::unordered_map<Point, int, TestType> m;

    Point a {1, 2};
    Point b {3, 4};
    Point c {5, 6};
    Point d {1, 2};

    SECTION("Inserting and retrieving elements")
    {
        m[a] = 10;
        m.insert({b, 20});

        REQUIRE(m.size() == 2);
        REQUIRE(m[a] == 10);
        REQUIRE(m.at(b) == 20);
    }

    SECTION("Duplicate key handling")
    {
        m[a] = 10;
        m.insert({b, 20});
        m.insert({c, 20});

        auto [it, inserted] = m.insert({d, 99});

        REQUIRE_FALSE(inserted);
        REQUIRE(m.size() == 3);
        REQUIRE(m[a] == 10);
    }

    SECTION("Accessing non-existent keys")
    {
        REQUIRE_THROWS_AS(m.at({99, 98}), std::out_of_range);
        REQUIRE(m[{3, 5}] == 0);
        REQUIRE(m.size() == 1);
    }
}

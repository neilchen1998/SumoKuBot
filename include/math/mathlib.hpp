#ifndef INCLUDE_MATH_MATHLIB_H_
#define INCLUDE_MATH_MATHLIB_H_

#include <cstddef>  // std::size_t
#include <span> // std::span
#include <functional>   // std::hash

#include <boost/container_hash/hash.hpp>    // boost::hash_combine

struct BoostPointHasher
{
    std::size_t operator()(const Point& p) const noexcept
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, p.x);
        boost::hash_combine(seed, p.y);
        return seed;
    }
};

/// @brief Defines a type that is hashable
template<typename T>
concept Hashable = requires (T a)
{
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

/// @brief Hashes a value with a seed
/// @tparam T Hashable
/// @param seed The seed
/// @param v The value
template <Hashable T>
void hash_combine(std::size_t& seed, const T& v)
{
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct PointHasher
{
    std::size_t operator()(const Point& p) const noexcept
    {
        std::size_t seed = 0;
        hash_combine(seed, p.x);
        hash_combine(seed, p.y);
        return seed;
    }
};

#endif // INCLUDE_MATH_MATHLIB_H_

#include <boost/container_hash/hash.hpp> // boost::hash_combine

#include "board/boardlib.hpp" // Point

/// @brief Hash two points using Boost library
struct BoostPointHasher
{
    std::size_t operator()(const Point &p) const noexcept
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, p.x);
        boost::hash_combine(seed, p.y);
        return seed;
    }
};

#include "arguments.hpp"

#include <CLI/CLI.hpp>     // CLI::App, CLI::ParseError
#include <array>           // std::array
#include <chrono>          // std::chrono::steady_clock
#include <concepts>        // concept
#include <cstdint>         // std::uint8_t
#include <cstdlib>         // EXIT_SUCCESS, EXIT_FAILURE
#include <filesystem>      // std::filesystem
#include <fmt/core.h>      // fmt::print
#include <fmt/ostream.h>   // fmt::streamed
#include <fmt/std.h>       // fmt::println for filesystem
#include <spdlog/spdlog.h> // spdlog::set_level, spdlog::info
#include <sstream>         // std::ostringstream
#include <string>          // std::string
#include <string_view>     // std::string_view
#include <utility>         // std::move
#include <vector>          // std::vector

/// @brief Overloads the stream insertion operator to convert Solvers enum value to string
/// @param os The output stream
/// @param s The solver type enum value
/// @return A reference to the output stream
std::ostream& operator<<(std::ostream& os, const SolverType& s)
{
    switch (s)
    {
    case SolverType::SumokuSolver:
    {
        return os << "Basic";
    }
    case SolverType::SumokuMRV:
    {
        return os << "SumokuMRV";
    }
    case SolverType::SumokuOrdering:
    {
        return os << "SumokuOrdering";
    }
    default:
    {
        return os << "Unknown";
    }
    }
}

#ifndef APPS_CLI_ARGUMENTS_HPP
#define APPS_CLI_ARGUMENTS_HPP

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

/// @brief The solver type
enum class SolverType : std::uint8_t // packs this enum inside a single byte type
{
    SumokuSolver,
    SumokuMRV,
    SumokuOrdering
};

/// @brief A map for all the solver types
constexpr std::array<std::pair<std::string_view, SolverType>, 3> solverMap {{
    {"Basic", SolverType::SumokuSolver},
    {"SumokuMRV", SolverType::SumokuMRV},
    {"SumokuOrdering", SolverType::SumokuOrdering},
}};

/// @brief Generates the CLI11 option text string dynamically from the solver map at compile time.
/// @return A formatted string listing all available solver options.
constexpr std::string GetSolverOptionsText()
{
    std::ostringstream oss;
    bool first = true;

    oss << "{";
    for (const auto& [name, _] : solverMap)
    {
        if (!first)
        {
            oss << ", ";
        }
        oss << name;
        first = false;
    }

    oss << "}";

    return oss.str();
}

/// @brief Overloads the stream insertion operator to convert Solvers enum value to string
/// @param os The output stream
/// @param s The solver type enum value
/// @return A reference to the output stream
std::ostream& operator<<(std::ostream& os, const SolverType& s);

#endif // APPS_CLI_ARGUMENTS_HPP

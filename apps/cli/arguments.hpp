#ifndef APPS_CLI_ARGUMENTS_HPP
#define APPS_CLI_ARGUMENTS_HPP

#include <array>       // std::array
#include <cstdint>     // std::uint8_t
#include <string_view> // std::string_view
#include <utility>     // std::pair

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
consteval auto GetSolverOptionsText()
{
    constexpr std::size_t maxSize = 128;
    std::array<char, maxSize> buffer {};

    std::size_t pos = 0;
    buffer[pos++] = '{';

    bool first = true;
    for (const auto& [name, _] : solverMap)
    {
        if (!first)
        {
            buffer[pos++] = ',';
            buffer[pos++] = ' ';
        }

        for (char c : name)
        {
            buffer[pos++] = c;
        }

        first = false;
    }

    buffer[pos++] = '}';

    return std::pair {buffer, pos};
}

/// @brief Overloads the stream insertion operator to convert Solvers enum value to string
/// @param os The output stream
/// @param s The solver type enum value
/// @return A reference to the output stream
std::ostream& operator<<(std::ostream& os, const SolverType& s);

#endif // APPS_CLI_ARGUMENTS_HPP

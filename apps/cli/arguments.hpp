#ifndef APPS_CLI_ARGUMENTS_HPP
#define APPS_CLI_ARGUMENTS_HPP

#include <array>           // std::array
#include <cstdint>         // std::uint8_t
#include <sstream>         // std::ostringstream
#include <string>          // std::string

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

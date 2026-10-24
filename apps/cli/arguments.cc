#include "arguments.hpp"

#include <ostream> // std::ostream

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

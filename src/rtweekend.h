#pragma once

// No warnings from external headers
#pragma warning(push, 0)

#include <limits>

#pragma warning(pop)

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

#pragma once

#include <limits>
#include <random>

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

// This is not threadsafe, rand() is thread safe depending on implementation
// I'm not going to fight for this for now
inline double random_double()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    static auto rand_generator = [&]()
    {
        return distribution(generator);
    };
    return rand_generator();
}

inline double random_double(double min, double max)
{
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline int random_int()
{
    static std::uniform_int_distribution<int> distribution(0, 1);
    static std::mt19937 generator;
    static auto rand_generator = [&]()
    {
        return distribution(generator);
    };
    return rand_generator();
}

inline int random_int(int min, int max)
{
    return min + (max - min) * random_int();
}


inline double clamp(double x, double min, double max)
{
    if (x < min)
    {
        return min;
    }
    else if (x > max)
    {
        return max;
    }
    else
    {
        return x;
    }
}

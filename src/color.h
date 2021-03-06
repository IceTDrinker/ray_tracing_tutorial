#pragma once

#include <iostream>
#include <vector>

#include "rtweekend.h"

#include "vec3.h"

void write_color(std::ostream& out, color pixel_color, int samples_per_pixel)
{
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color total by the number of samples.
    auto scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

void write_color(unsigned char pixel_data[3], color pixel_color, int samples_per_pixel)
{
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color total by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = std::sqrt(r * scale);
    g = std::sqrt(g * scale);
    b = std::sqrt(b * scale);

    pixel_data[0] = static_cast<unsigned char>(256 * clamp(r, 0.0, 0.999));
    pixel_data[1] = static_cast<unsigned char>(256 * clamp(g, 0.0, 0.999));
    pixel_data[2] = static_cast<unsigned char>(256 * clamp(b, 0.0, 0.999));
}

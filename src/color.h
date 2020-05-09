#pragma once

#include "vec3.h"

#include <iostream>
#include <vector>

void write_color(std::ostream& out, color pixel_color)
{
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixel_color.x()) << ' '
        << static_cast<int>(255.999 * pixel_color.y()) << ' '
        << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

void write_color(unsigned char pixel_data[3], color pixel_color)
{
    pixel_data[0] = static_cast<unsigned char>(255.999 * pixel_color.x());
    pixel_data[1] = static_cast<unsigned char>(255.999 * pixel_color.y());
    pixel_data[2] = static_cast<unsigned char>(255.999 * pixel_color.z());
}

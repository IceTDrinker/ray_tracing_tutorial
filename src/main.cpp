// No warnings from external headers
#pragma warning(push, 0)

#include <cstdlib>
#include <ctime>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace fs = std::filesystem;

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#pragma warning(pop)

#include "color.h"
#include "ray.h"

static constexpr const char* output_dir = "outputs/";

// Source : https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
// Get current date/time, format is YYYY-MM-DD_HH-mm-ss
const std::string currentDateTime()
{
    time_t     now = std::time(0);
    struct tm  tstruct;
    char       buf[128];
    localtime_s(&tstruct, &now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &tstruct);

    return buf;
}

double hit_sphere(const point3& center, double radius, const ray& r)
{
    vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        return (-b - sqrt(discriminant)) / (2.0 * a);
    }
}

color ray_color(const ray& r)
{
    auto t = hit_sphere(point3(0, 0, -1), 0.5, r);
    if (t > 0.0)
    {
        vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
        return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
    }

    vec3 unit_direction = unit_vector(r.direction());
    t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main(int /*argc*/, char* /*argv[]*/)
{
    static constexpr const auto aspect_ratio = 16.0 / 9.0;
    static constexpr const int image_width = 1920;
    static constexpr const int image_height = static_cast<int>(image_width / aspect_ratio);
    static constexpr const int num_channels = 3;

    auto image = std::vector<unsigned char>(image_width * image_height * num_channels);

    auto output_dir_path = fs::path(output_dir);

    if (!fs::exists(output_dir_path))
    {
        auto creation_res = fs::create_directories(output_dir_path);

        if (!creation_res)
        {
            throw std::runtime_error("Unable to create output directory");
        }
    }

    auto out_filename = output_dir + currentDateTime() + ".png";

    point3 origin(0.0, 0.0, 0.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, horizontal.x() / aspect_ratio, 0.0);
    point3 lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, 1);

    for (int j = image_height - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            auto u = double(i) / (image_width - 1);
            auto v = double(image_height - 1 - j) / (image_height - 1); // Image data is inverted on the y axis because of chosen coordinate system
            ray r(origin, lower_left_corner + u * horizontal + v * vertical);
            color pixel_color = ray_color(r);
            auto pixel_index = num_channels * (j * image_width + i);
            write_color(&image.data()[pixel_index], pixel_color);
        }
    }

    stbi_write_png(out_filename.c_str(), image_width, image_height, 3, image.data(), image_width * num_channels * sizeof(unsigned char));

    std::cerr << "\nDone.\n";

    return EXIT_SUCCESS;
}

// No warnings from external headers
#pragma warning(push, 0)

#include <cstdlib>
#include <ctime>

#include <fstream>
#include <iostream>

#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

#pragma warning(pop)

#include "color.h"
#include "ray.h"

static constexpr const char* output_dir = "outputs";

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

bool hit_sphere(const point3& center, double radius, const ray& r)
{
    vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = b * b - 4 * a * c;
    return (discriminant > 0);
}

color ray_color(const ray& r)
{
    if (hit_sphere(point3(0, 0, -1), 0.5, r))
    {
        return color(1, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main(int /*argc*/, char* /*argv[]*/)
{
    static constexpr const auto aspect_ratio = 16.0 / 9.0;
    static constexpr const int image_width = 384;
    static constexpr const int image_height = static_cast<int>(image_width / aspect_ratio);

    auto output_dir_path = fs::path(output_dir);

    if (!fs::exists(output_dir_path))
    {
        auto creation_res = fs::create_directories(output_dir_path);

        if (!creation_res)
        {
            throw std::runtime_error("Unable to create output directory");
        }
    }

    auto out_filename = currentDateTime() + ".ppm";
    auto out_file_path = output_dir_path.append(out_filename);
    auto output_file = std::ofstream(out_file_path);
    auto& outstream = output_file;

    outstream << "P3\n" << image_width << ' ' << image_height << "\n255\n";

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
            auto v = double(j) / (image_height - 1);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical);
            color pixel_color = ray_color(r);
            write_color(outstream, pixel_color);
        }
        outstream << "\n";
    }

    std::cerr << "\nDone.\n";

    return EXIT_SUCCESS;
}

// No warnings from external headers
#pragma warning(push, 0)

#include <cmath>
#include <cstdlib>
#include <ctime>

#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace fs = std::filesystem;

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#pragma warning(pop)

#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "ray.h"
#include "sphere.h"
#include "vec3.h"

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

color ray_color(const ray& r, const hittable& world)
{
    hit_record rec;
    if (world.hit(r, 0, infinity, rec))
    {
        return 0.5 * (rec.normal + color(1, 1, 1));
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main(int /*argc*/, char* /*argv[]*/)
{
    static constexpr const auto aspect_ratio = 16.0 / 9.0;
    static constexpr const int image_width = 1920;
    static constexpr const int image_height = static_cast<int>(image_width / aspect_ratio);
    static constexpr const int num_channels = 3;
    static constexpr const int samples_per_pixel = 100;

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

    hittable_list world;
    world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(std::make_shared<sphere>(point3(0, -100.5, -1), 100));
    camera cam{aspect_ratio};

    for (int j = image_height - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s)
            {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (image_height - 1 - j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world);
            }
            auto pixel_index = num_channels * (j * image_width + i);
            write_color(&image.data()[pixel_index], pixel_color, samples_per_pixel);
        }
    }

    stbi_write_png(out_filename.c_str(), image_width, image_height, 3, image.data(), image_width * num_channels * sizeof(unsigned char));

    std::cerr << "\nDone.\n";

    return EXIT_SUCCESS;
}

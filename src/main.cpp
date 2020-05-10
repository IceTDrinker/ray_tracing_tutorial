#include <cmath>
#include <cstdlib>
#include <ctime>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

// No warnings from external headers
#pragma warning(push, 0)

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#pragma warning(pop)

#include "rtweekend.h"

#include "bvh.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "moving_sphere.h"
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

color ray_color(const ray& r, const hittable& world, int depth)
{
    hit_record rec;
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
    {
        return color(0, 0, 0);
    }

    if (world.hit(r, 0.001, infinity, rec))
    {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * ray_color(scattered, world, depth - 1);
        }
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

hittable_list random_scene()
{
    hittable_list world;

    world.add(std::make_shared<sphere>(point3(0, -1000, 0), 1000, std::make_shared<lambertian>(color(0.5, 0.5, 0.5))));

    for (int a = -10; a < 10; a++)
    {
        for (int b = -10; b < 10; b++)
        {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9)
            {
                if (choose_mat < 0.8)
                {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    world.add(std::make_shared<moving_sphere>(
                        center, center + vec3(0, random_double(0.0, 0.5), 0.0), 0.0, 1.0, 0.2,
                        std::make_shared<lambertian>(albedo)));
                }
                else if (choose_mat < 0.95)
                {
                    // metal
                    auto albedo = color::random(0.5, 1.0);
                    auto fuzz = random_double(0.0, 0.5);
                    world.add(
                        std::make_shared<sphere>(center, 0.2, std::make_shared<metal>(albedo, fuzz)));
                }
                else
                {
                    // glass
                    world.add(std::make_shared<sphere>(center, 0.2, std::make_shared<dielectric>(1.5)));
                }
            }
        }
    }

    world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, std::make_shared<dielectric>(1.5)));
    world.add(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, std::make_shared<lambertian>(color(0.4, 0.2, 0.1))));
    world.add(std::make_shared<sphere>(point3(4, 1, 0), 1.0, std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0)));

    return world;
}

int main(int /*argc*/, char* /*argv[]*/)
{
    static constexpr const auto aspect_ratio = 16.0 / 9.0;
    static constexpr const int image_width = 1920;
    static constexpr const int image_height = static_cast<int>(image_width / aspect_ratio);
    static constexpr const int num_channels = 3;
    static constexpr const int samples_per_pixel = 100;
    static constexpr const int num_threads = 8;
    static constexpr const int max_depth = 50;

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

    // Camera with exposure time
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    double time0 = 0.0;
    double time1 = 1.0;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, time0, time1);

    // Defocus blur aka depth of field
    //point3 lookfrom(13, 2, 3);
    //point3 lookat(0, 0, 0);
    //vec3 vup(0, 1, 0);
    //auto dist_to_focus = 10.0;
    //auto aperture = 0.1;

    //camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    auto world_hittable_list = random_scene();
    auto world = bvh_node(world_hittable_list, time0, time1);

    //hittable_list world;
    //world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5, std::make_shared<lambertian>(color(0.1, 0.2, 0.5))));
    //world.add(std::make_shared<sphere>(point3(0, -100.5, -1), 100, std::make_shared<lambertian>(color(0.8, 0.8, 0.0))));
    //world.add(std::make_shared<sphere>(point3(1, 0, -1), 0.5, std::make_shared<metal>(color(.8, .6, .2), 0.0)));
    //world.add(std::make_shared<sphere>(point3(-1, 0, -1), 0.5, std::make_shared<dielectric>(1.5)));
    //world.add(std::make_shared<sphere>(point3(-1, 0, -1), -0.45, std::make_shared<dielectric>(1.5)));

    //auto R = cos(pi / 4);
    //hittable_list world;
    //world.add(std::make_shared<sphere>(point3(-R, 0, -1), R, std::make_shared<lambertian>(color(0, 0, 1))));
    //world.add(std::make_shared<sphere>(point3(R, 0, -1), R, std::make_shared<lambertian>(color(1, 0, 0))));

    auto process_rows = [&](int start_j, int stop_j)
    {
        for (int j = stop_j - 1; j >= start_j; --j)
        {
            //std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i)
            {
                color pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel; ++s)
                {
                    auto u = (i + random_double()) / (image_width - 1);
                    // Axis y is inverted in conventional images (a png is written below), invert j
                    auto inverted_j = image_height - 1 - j;
                    auto v = (inverted_j + random_double()) / (image_height - 1);
                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, world, max_depth);
                }
                auto pixel_index = num_channels * (j * image_width + i);
                write_color(&image.data()[pixel_index], pixel_color, samples_per_pixel);
            }
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    auto work_size = image_height / num_threads;
    auto remaining_work = image_height % num_threads;

    auto start = std::chrono::high_resolution_clock::now();

    for (auto thread_idx = 0; thread_idx < num_threads; ++thread_idx)
    {
        auto start_row = thread_idx * work_size;
        auto stop_row = start_row + work_size;
        if (thread_idx < num_threads - 1)
        {
            threads.push_back(std::thread(process_rows, start_row, stop_row));
        }
        else
        {
            threads.push_back(std::thread(process_rows, start_row, stop_row + remaining_work));
        }
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cerr << "Ray tracing took : " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds" << std::endl;

    stbi_write_png(out_filename.c_str(), image_width, image_height, 3, image.data(), image_width * num_channels * sizeof(unsigned char));

    std::cerr << "Done." << std::endl;;

    return EXIT_SUCCESS;
}

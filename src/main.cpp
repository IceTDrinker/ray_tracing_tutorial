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

int main(int /*argc*/, char* /*argv[]*/)
{
    static constexpr const int image_width = 256;
    static constexpr const int image_height = 256;

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

    for (int j = image_height - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            color pixel_color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.25);
            write_color(outstream, pixel_color);
        }
        outstream << "\n";
    }

    std::cerr << "\nDone.\n";

    return EXIT_SUCCESS;
}

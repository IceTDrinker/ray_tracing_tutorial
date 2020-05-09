#pragma once

#include "ray.h"
#include "vec3.h"

class camera
{
public:
    camera(double aspect_ratio)
    {
        origin = point3(0.0, 0.0, 0.0);
        horizontal = vec3(4.0, 0.0, 0.0);
        vertical = vec3(0.0, horizontal.x() / aspect_ratio, 0.0);
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, 1);
    }

    ray get_ray(double u, double v) const
    {
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};

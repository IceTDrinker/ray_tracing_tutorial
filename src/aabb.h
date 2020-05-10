#pragma once

#include <cmath>

#include "rtweekend.h"

#include "ray.h"
#include "vec3.h"

class aabb
{
public:
    aabb() {}
    aabb(const point3& a, const point3& b)
    {
        point3 small(std::fmin(a.x(), b.x()),
                     std::fmin(a.y(), b.y()),
                     std::fmin(a.z(), b.z()));

        point3 big(std::fmax(a.x(), b.x()),
                   std::fmax(a.y(), b.y()),
                   std::fmax(a.z(), b.z()));

        _min = small;
        _max = big;
    }

    point3 min() const { return _min; }
    point3 max() const { return _max; }

    bool hit(const ray& r, double tmin, double tmax) const
    {
        for (int a = 0; a < 3; a++)
        {
            auto invD = 1.0f / r.direction()[a];
            auto t0 = (min()[a] - r.origin()[a]) * invD;
            auto t1 = (max()[a] - r.origin()[a]) * invD;
            if (invD < 0.0f)
            {
                std::swap(t0, t1);
            }
            tmin = t0 > tmin ? t0 : tmin;
            tmax = t1 < tmax ? t1 : tmax;
            if (tmax <= tmin)
            {
                return false;
            }
        }
        return true;
    }

    point3 _min;
    point3 _max;
};

aabb surrounding_box(aabb box0, aabb box1)
{
    point3 small(std::fmin(box0.min().x(), box1.min().x()),
                 std::fmin(box0.min().y(), box1.min().y()),
                 std::fmin(box0.min().z(), box1.min().z()));

    point3 big(std::fmax(box0.max().x(), box1.max().x()),
               std::fmax(box0.max().y(), box1.max().y()),
               std::fmax(box0.max().z(), box1.max().z()));

    return aabb(small, big);
}

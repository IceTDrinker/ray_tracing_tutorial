#pragma once

#include <memory>

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable
{
public:
    sphere() = delete;
    sphere(point3 cen, double r, std::shared_ptr<material> m)
        : center(cen), radius(r), mat_ptr(m)
    {
    };

    virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const;

public:
    point3 center;
    double radius;
    std::shared_ptr<material> mat_ptr;
};

inline bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = half_b * half_b - a * c;

    if (discriminant > 0)
    {
        auto root = std::sqrt(discriminant);
        auto temp = (-half_b - root) / a;

        auto fill_sphere_hit_record = [&]()
        {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
        };

        if (temp < t_max && temp > t_min)
        {
            fill_sphere_hit_record();
            return true;
        }
        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min)
        {
            fill_sphere_hit_record();
            return true;
        }
    }
    return false;
}

bool sphere::bounding_box(double /*t0*/, double /*t1*/, aabb& output_box) const
{
    output_box = aabb(
        center - vec3(radius, radius, radius),
        center + vec3(radius, radius, radius));
    return true;
}

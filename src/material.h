#pragma once

#include <memory>

#include "rtweekend.h"
#include "texture.h"

struct hit_record;

class material
{
public:
  virtual color emitted(double u, double v, const point3& p) const
  {
    return color(0, 0, 0);
  }

  virtual bool scatter(const ray& r_in,
                       const hit_record& rec,
                       color& attenuation,
                       ray& scattered) const = 0;
};

class lambertian : public material
{
public:
  lambertian(const color& a)
    : albedo(make_shared<solid_color>(a))
  {}
  lambertian(shared_ptr<texture> a)
    : albedo(a)
  {}

  virtual bool scatter(const ray& r_in,
                       const hit_record& rec,
                       color& attenuation,
                       ray& scattered) const override
  {
    auto scatter_direction = rec.normal + random_unit_vector();

    // Catch degenerate scatter direction
    if (scatter_direction.near_zero())
      scatter_direction = rec.normal;

    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo->value(rec.u, rec.v, rec.p);
    return true;
  }

public:
  shared_ptr<texture> albedo;
};

class metal : public material
{
public:
  metal(const color& a, double f)
    : albedo(a)
    , fuzz(f < 1 ? f : 1)
  {}

  virtual bool scatter(const ray& r_in,
                       const hit_record& rec,
                       color& attenuation,
                       ray& scattered) const override
  {
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

public:
  color albedo;
  double fuzz;
};

class dielectric : public material
{
public:
  dielectric(double index_of_refraction)
    : ir(index_of_refraction)
  {}

#define RA 0.01

  virtual bool scatter(const ray& r_in,
                       const hit_record& rec,
                       color& attenuation,
                       ray& scattered) const override
  {
    attenuation = color(1.0, 1.0, 1.0);
    auto ir_tmp = ir;
    if (r_in.rgb_ == RGB::R) {
      ir_tmp += RA + (rand() / double(RAND_MAX) - 0.5) * RA;
    } else if (r_in.rgb_ == RGB::B) {
      ir_tmp += -RA + (rand() / double(RAND_MAX) - 0.5) * RA;
    } else {
      ir_tmp += 0 + (rand() / double(RAND_MAX) - 0.5) * RA;
    }
    double refraction_ratio = rec.front_face ? (1.0 / ir_tmp) : ir_tmp;

    vec3 unit_direction = unit_vector(r_in.direction());
    double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    vec3 direction;

    if (cannot_refract ||
        reflectance(cos_theta, refraction_ratio) > random_double())
      direction = reflect(unit_direction, rec.normal);
    else
      direction = refract(unit_direction, rec.normal, refraction_ratio);

    scattered = ray(rec.p, direction);

    return true;
  }

public:
  double ir; // Index of Refraction

private:
  static double reflectance(double cosine, double ref_idx)
  {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
  }
};

class diffuse_light : public material
{
public:
  diffuse_light(shared_ptr<texture> a)
    : emitt(a)
  {}
  diffuse_light(color c)
    : emitt(make_shared<solid_color>(c))
  {}

  virtual bool scatter(const ray& r_in,
                       const hit_record& rec,
                       color& attenuation,
                       ray& scattered) const override
  {
    return false;
  }

  virtual color emitted(double u, double v, const point3& p) const override
  {
    return emitt->value(u, v, p);
  }

public:
  shared_ptr<texture> emitt;
};

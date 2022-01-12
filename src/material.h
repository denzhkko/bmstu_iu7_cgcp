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

  virtual std::string about() const { return "нет информации по материалу"; }
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

  std::string about() const override
  {
    return QString("матовый (текстура: %1)")
      .arg(QString::fromStdString(albedo->about()))
      .toStdString();
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

  std::string about() const override { return "металл"; }

public:
  color albedo;
  double fuzz;
};

class dielectric : public material
{
public:
  dielectric(std::array<double, 3> b, std::array<double, 3> c)
    : b_{ b }
    , c_{ c }
  {}

#define RA 0.01

  virtual bool scatter(const ray& r_in,
                       const hit_record& rec,
                       color& attenuation,
                       ray& scattered) const override
  {
    attenuation = color(1.0, 1.0, 1.0);
    double ray_len = 0.0;
    char color = 'a';
    if (r_in.rgb_ == RGB::R) {
      color = 'r';
      // 630-780
      ray_len = 630 + rand() % (780 - 630);
    } else if (r_in.rgb_ == RGB::B) {
      color = 'b';
      // 450-480
      ray_len = 450 + rand() % (480 - 450);
    } else {
      color = 'g';
      // 510-550
      ray_len = 510 + rand() % (550 - 510);
    }
    ray_len /= 1e3;

    auto n =
      std::sqrt(1 + (b_[0] * ray_len * ray_len) / (ray_len * ray_len - c_[0]) +
                (b_[1] * ray_len * ray_len) / (ray_len * ray_len - c_[1]) +
                (b_[2] * ray_len * ray_len) / (ray_len * ray_len - c_[2]));
    double refraction_ratio = rec.front_face ? (1.0 / n) : n;

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

  std::string about() const override { return "прозрачный"; }

public:
  std::array<double, 3> b_;
  std::array<double, 3> c_;

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

  std::string about() const override { return "источник света"; }

public:
  shared_ptr<texture> emitt;
};

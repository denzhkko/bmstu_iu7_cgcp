#pragma once

#include "vec3.h"

enum class RGB
{
  R,
  G,
  B
};

class ray
{
public:
  ray(RGB const rgb = RGB::R)
    : rgb_{ rgb }
  {}
  ray(const point3& origin, const vec3& direction, RGB const rgb = RGB::R)
    : orig(origin)
    , dir(direction)
    , rgb_{ rgb }
  {}

  point3 origin() const { return orig; }
  vec3 direction() const { return dir; }

  point3 at(double t) const { return orig + t * dir; }

  void set_RGB(RGB const rgb) { rgb_ = rgb; }

public:
  point3 orig;
  vec3 dir;
  RGB rgb_ = RGB::R;
};

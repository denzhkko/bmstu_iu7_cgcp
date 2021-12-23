#pragma once

struct settings_render
{
public:
  settings_render(unsigned width, unsigned height, unsigned ray_pp)
    : width_{ width }
    , height_{ height }
    , ray_pp_{ ray_pp }
  {}

  unsigned width_ = 0;
  unsigned height_ = 0;
  unsigned ray_pp_ = 100;
};

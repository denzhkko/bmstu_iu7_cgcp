#pragma once

class render_settings
{
public:
  render_settings(unsigned width, unsigned height, unsigned ray_pp)
    : width_{ width }
    , height_{ height }
    , ray_pp_{ ray_pp }
  {}

  unsigned width_ = 0;
  unsigned height_ = 0;
  unsigned ray_pp_ = 100;
};

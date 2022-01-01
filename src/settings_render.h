#pragma once

struct settings_render
{
public:
  settings_render(unsigned width,
                  unsigned height,
                  unsigned ray_pp,
                  double camera_canvas)
    : width_{ width }
    , height_{ height }
    , ray_pp_{ ray_pp }
    , camera_canvas_{ camera_canvas }
  {}

  unsigned width_ = 0;
  unsigned height_ = 0;
  unsigned ray_pp_ = 100;
  double camera_canvas_ = 1.0;
};

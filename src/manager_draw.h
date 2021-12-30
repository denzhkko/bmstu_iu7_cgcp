#pragma once

#include <QImage>
#include <functional> // function

#include "scene.h"
#include "settings_render.h"

class manager_draw
{
public:
  void draw(settings_render const rs,
            scene scene,
            std::function<void(double progress)> notify_progress,
            std::function<bool()> is_cancelled,
            std::function<void(QImage)> send_pic);

private:
};

color
ray_color(const ray& r,
          const color& background,
          const hittable& world,
          int depth);
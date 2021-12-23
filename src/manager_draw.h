#pragma once

#include <QImage>
#include <functional> // function
#include "render_settings.h"

class manager_draw
{
public:
  void draw(render_settings const rs,
            std::function<void(double progress)> notify_progress,
            std::function<bool()> is_cancelled,
            std::function<void(QImage)> send_pic);

private:
};

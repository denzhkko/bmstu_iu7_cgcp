#pragma once

#include <QImage>
#include <functional> // function

class manager_draw
{
public:
  void draw(unsigned const width,
            unsigned const height,
            std::function<void(double progress)> notify_progress,
            std::function<bool()> is_cancelled,
            std::function<void(QImage)> send_pic);

private:
};

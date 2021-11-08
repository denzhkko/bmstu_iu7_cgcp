#include "manager_draw.h"

#include <chrono>    // duration
#include <iostream>  // cout
#include <thread>    // thread

#include "ray.h"
#include "vec3.h"

using namespace std::literals::chrono_literals;

double hit_sphere(const point3& center, double radius, const ray& r) {
  vec3 oc = r.origin() - center;
  auto a = r.direction().length_squared();
  auto half_b = dot(oc, r.direction());
  auto c = oc.length_squared() - radius * radius;
  auto discriminant = half_b * half_b - a * c;

  if (discriminant < 0) {
    return -1.0;
  } else {
    return (-half_b - sqrt(discriminant)) / a;
  }
}

color ray_color(const ray& r) {
  auto t = hit_sphere(point3(0, 0, -1), 0.5, r);
  if (t > 0.0) {
    vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
    return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
  }
  vec3 unit_direction = unit_vector(r.direction());
  t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

void manager_draw::draw(unsigned const width, unsigned const height,
                        std::function<void(double progress)> notify_progress,
                        std::function<bool()> is_cancelled,
                        std::function<void(QImage)> send_pic) {
  auto th = std::thread(
      [notify_progress, is_cancelled, send_pic](int img_w, int img_h) {
        QImage image(img_w, img_h, QImage::Format::Format_ARGB32_Premultiplied);
        image.fill(QColor(255, 255, 255));

        const auto aspect_ratio = static_cast<double>(img_w) / img_h;

        auto viewport_height = 2.0;
        auto viewport_width = aspect_ratio * viewport_height;
        auto focal_length = 1.0;

        auto origin = point3(0, 0, 0);
        auto horizontal = vec3(viewport_width, 0, 0);
        auto vertical = vec3(0, viewport_height, 0);
        auto lower_left_corner =
            origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

        for (int j = img_h - 1; j >= 0; --j) {
          for (int i = 0; i < img_w; ++i) {
            auto u = double(i) / (img_w - 1);
            auto v = double(j) / (img_h - 1);
            ray r(origin,
                  lower_left_corner + u * horizontal + v * vertical - origin);
            color c = ray_color(r);

            image.setPixelColor(i, j,
                                {static_cast<int>(255.999 * c.x()),
                                 static_cast<int>(255.999 * c.y()),
                                 static_cast<int>(255.999 * c.z())});

            double progress =
                100.0 - (100.0 * (j * img_w + img_w - i)) / (img_h * img_w);
            notify_progress(progress);
          }
          if (is_cancelled()) break;
        }

        send_pic(image);
      },
      width, height);

  th.detach();
}

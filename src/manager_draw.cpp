#include "manager_draw.h"

#include <chrono>    // duration
#include <iostream>  // cout
#include <thread>    // thread

#include "camera.h"
#include "hittable_list.h"
#include "ray.h"
#include "rtweekend.h"
#include "sphere.h"
#include "vec3.h"

using namespace std::literals::chrono_literals;

color ray_color(const ray& r, const hittable& world) {
  hit_record rec;
  if (world.hit(r, 0, infinity, rec)) {
    return 0.5 * (rec.normal + color(1, 1, 1));
  }
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
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

        // Image
        const auto aspect_ratio = static_cast<double>(img_w) / img_h;
        const int samples_per_pixel = 100;

        // World
        hittable_list world;
        world.add(std::make_shared<sphere>(point3{0, 0, -1}, 0.5));
        world.add(std::make_shared<sphere>(point3{0, -100.5, -1}, 100));

        // Camera
        camera cam(aspect_ratio);

        for (int j = img_h - 1; j >= 0; --j) {
          for (int i = 0; i < img_w; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
              auto u = (i + random_double()) / (img_w - 1);
              auto v = (j + random_double()) / (img_h - 1);
              ray r = cam.get_ray(u, v);
              pixel_color += ray_color(r, world);
            }

            pixel_color /= samples_per_pixel;

            image.setPixelColor(
                i, j,
                {static_cast<int>(256 * clamp(pixel_color.x(), 0.0, 0.999)),
                 static_cast<int>(256 * clamp(pixel_color.y(), 0.0, 0.999)),
                 static_cast<int>(256 * clamp(pixel_color.z(), 0.0, 0.999))});

            double progress =
                100.0 - (100.0 * (j * img_w + img_w - i)) / (img_h * img_w);
            notify_progress(progress);
          }
          if (is_cancelled()) break;
        }

        image = image.mirrored(false, true);
        send_pic(image);
      },
      width, height);

  th.detach();
}

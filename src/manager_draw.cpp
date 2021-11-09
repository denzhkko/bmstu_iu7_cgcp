#include "manager_draw.h"

#include <chrono>    // duration
#include <iostream>  // cout
#include <thread>    // thread

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "ray.h"
#include "rtweekend.h"
#include "sphere.h"
#include "vec3.h"

using namespace std::literals::chrono_literals;

color ray_color(const ray& r, const hittable& world, int depth) {
  hit_record rec;

  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0) return color(0, 0, 0);

  if (world.hit(r, 0.001, infinity, rec)) {
    point3 target = rec.p + random_in_hemisphere(rec.normal);
    return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
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
        const int max_depth = 50;

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
              pixel_color += ray_color(r, world, max_depth);
            }

            // write_color(std::cout, pixel_color, samples_per_pixel);

            auto r = pixel_color.x();
            auto g = pixel_color.y();
            auto b = pixel_color.z();

            // Divide the color by the number of samples and gamma-correct for
            // gamma=2.0.
            auto scale = 1.0 / samples_per_pixel;
            r = sqrt(scale * r);
            g = sqrt(scale * g);
            b = sqrt(scale * b);

            image.setPixelColor(i, j,
                                {static_cast<int>(256 * clamp(r, 0.0, 0.999)),
                                 static_cast<int>(256 * clamp(g, 0.0, 0.999)),
                                 static_cast<int>(256 * clamp(b, 0.0, 0.999))});

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

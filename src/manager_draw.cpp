#include "manager_draw.h"

#include <atomic>
#include <boost/log/trivial.hpp>
#include <chrono>   // duration
#include <iostream> // cout
#include <thread>   // thread

#include "bvh.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "ray.h"
#include "rtweekend.h"
#include "sphere.h"
#include "vec3.h"

using namespace std::literals::chrono_literals;

color
ray_color(const ray& r,
          const color& background,
          const hittable& world,
          int depth)
{
  hit_record rec;

  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0)
    return color(0, 0, 0);

  // If the ray hits nothing, return the background color.
  if (!world.hit(r, 0.001, infinity, rec))
    return background;

  ray scattered;
  color attenuation;
  color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

  if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    return emitted;

  scattered.rgb_ = r.rgb_;

  return emitted +
         attenuation * ray_color(scattered, background, world, depth - 1);
}

void
manager_draw::draw(settings_render const rs,
                   scene scene,
                   std::function<void(double progress)> notify_progress,
                   std::function<bool()> is_cancelled,
                   std::function<void(QImage)> send_pic)
{
  auto th = std::thread(
    [notify_progress, is_cancelled, send_pic](settings_render rs,
                                              struct scene const& scene) {
      unsigned img_w = rs.width_;
      unsigned img_h = rs.height_;

      QImage image(img_w, img_h, QImage::Format::Format_ARGB32_Premultiplied);
      image.fill(QColor(255, 255, 255));

      // Image
      const auto aspect_ratio = static_cast<double>(img_w) / img_h;
      const int max_depth = 50;

      hittable_list objects;
      objects.add(make_shared<bvh_node>(scene.world_));

      // Camera
      camera cam(scene.lookfrom_,
                 scene.lookto_,
                 vec3(0, 1, 0),
                 45,
                 aspect_ratio,
                 rs.camera_canvas_);

      color background = scene.background_;

      unsigned u_progress = 0;
#pragma omp parallel for schedule(dynamic)
      for (int p = 0; p < img_w * img_h; ++p) {
        if (is_cancelled())
          continue;

        int i = p / img_w;
        int j = p % img_w;

        color pixel_color(0, 0, 0);
        for (int s = 0; s < rs.ray_pp_; ++s) {
          auto u = (j + random_double()) / (img_w - 1);
          auto v = (i + random_double()) / (img_h - 1);
          ray r = cam.get_ray(u, v);
          r.set_RGB(RGB::R);
          pixel_color.e[0] +=
            ray_color(r, background, scene.world_, max_depth).e[0];
          r.set_RGB(RGB::G);
          pixel_color.e[1] +=
            ray_color(r, background, scene.world_, max_depth).e[1];
          r.set_RGB(RGB::B);
          pixel_color.e[2] +=
            ray_color(r, background, scene.world_, max_depth).e[2];
        }

        auto r = pixel_color.x();
        auto g = pixel_color.y();
        auto b = pixel_color.z();

        // Divide the color by the number of samples and gamma-correct for
        // gamma=2.0.
        auto scale = 1.0 / rs.ray_pp_;
        r = sqrt(scale * r);
        g = sqrt(scale * g);
        b = sqrt(scale * b);

        image.setPixelColor(j,
                            i,
                            { static_cast<int>(256 * clamp(r, 0.0, 0.999)),
                              static_cast<int>(256 * clamp(g, 0.0, 0.999)),
                              static_cast<int>(256 * clamp(b, 0.0, 0.999)) });

#pragma omp critical
        {
          double progress = (100.0 * ++u_progress) / (img_h * img_w);
          notify_progress(progress);
        }
      }

      image = image.mirrored(false, true);
      send_pic(image);
    },
    rs,
    scene);

  th.detach();
}

#include "camera.h"
#include "manager_draw.h"
#include "material.h"
#include "sphere.h"
#include <boost/log/trivial.hpp>
#include <sstream>

QImage
render(settings_render const rs, scene scene)
{
  auto samples_per_pixel = rs.ray_pp_;
  int img_w = rs.width_;
  int img_h = rs.height_;

  QImage image(img_w, img_h, QImage::Format::Format_ARGB32_Premultiplied);
  image.fill(QColor(255, 255, 255));

  // Image
  const auto aspect_ratio = static_cast<double>(img_w) / img_h;
  const int max_depth = 50;

  // Camera
  camera cam(
    scene.lookfrom_, scene.lookto_, vec3(0, 1, 0), 45, aspect_ratio, 4.0);

  color background = scene.background_;

  for (int p = 0; p < img_w * img_h; ++p) {
    int i = p / img_w;
    int j = p % img_w;

    color pixel_color(0, 0, 0);
    for (int s = 0; s < samples_per_pixel; ++s) {
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
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    image.setPixelColor(j,
                        i,
                        { static_cast<int>(256 * clamp(r, 0.0, 0.999)),
                          static_cast<int>(256 * clamp(g, 0.0, 0.999)),
                          static_cast<int>(256 * clamp(b, 0.0, 0.999)) });

    double progress = (100.0 * p) / (img_h * img_w);
  }

  image = image.mirrored(false, true);

  return image;
}

QImage
render_ll(settings_render const rs, scene scene)
{
  auto samples_per_pixel = rs.ray_pp_;
  int img_w = rs.width_;
  int img_h = rs.height_;

  QImage image(img_w, img_h, QImage::Format::Format_ARGB32_Premultiplied);
  image.fill(QColor(255, 255, 255));

  // Image
  const auto aspect_ratio = static_cast<double>(img_w) / img_h;
  const int max_depth = 50;

  // Camera
  camera cam(
    scene.lookfrom_, scene.lookto_, vec3(0, 1, 0), 45, aspect_ratio, 4.0);

  color background = scene.background_;

#pragma omp parallel for schedule(dynamic)
  for (int p = 0; p < img_w * img_h; ++p) {
    int i = p / img_w;
    int j = p % img_w;

    color pixel_color(0, 0, 0);
    for (int s = 0; s < samples_per_pixel; ++s) {
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
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    image.setPixelColor(j,
                        i,
                        { static_cast<int>(256 * clamp(r, 0.0, 0.999)),
                          static_cast<int>(256 * clamp(g, 0.0, 0.999)),
                          static_cast<int>(256 * clamp(b, 0.0, 0.999)) });

    double progress = (100.0 * p) / (img_h * img_w);
  }

  image = image.mirrored(false, true);

  return image;
}

int
main()
{
  std::vector<std::pair<unsigned, unsigned>> img_sizes = {
    { 100, 100 }, { 200, 200 }, { 500, 500 }, { 1000, 1000 }, { 2000, 2000 }
  };
  std::vector<int> fig_cnt = {2, 4, 8, 16};

  std::vector<std::shared_ptr<material>> materials{};
  materials.push_back(make_shared<lambertian>(
    make_shared<checker_texture>(color(0, 0, 0), color(1, 1, 1))));
  materials.push_back(make_shared<metal>(color(0.8, 0.6, 0.2), 0.0));
  std::array<double, 3> b{ 1.03961212, 0.231792344, 1.01046945 };
  std::array<double, 3> c{ 6.00069867 * 10e-3,
                           2.00179144 * 10e-2,
                           1.03560653 * 10e2 };
  materials.push_back(make_shared<dielectric>(b, c));
  materials.push_back(
    make_shared<lambertian>(make_shared<solid_color>(color(0.8, 0.6, 0.2))));
  materials.push_back(make_shared<metal>(color(0.1, 0.2, 0.5), 0.1));

  for (auto cnt : fig_cnt) {
    BOOST_LOG_TRIVIAL(info) << "cnt: " << cnt;
    hittable_list world;

    for (auto i = 0; i < cnt; ++i) {
      int dx = 5, dy = 5, dz = 5;
      point3 center{ static_cast<double>(0 + rand() % (2 * dx) - dx),
                     static_cast<double>(0 + rand() % (2 * dy) - dy),
                     static_cast<double>(-10 + rand() % (2 * dz) - dz) };
      int radius = rand() % 3 + 1;

      world.add(std::make_shared<sphere>(
        center, radius, materials.at(rand() % materials.size())));
    }

    scene scene{ { 1, 1, 1 }, { 0, 0, 5 }, { 0, 0, 0 }, world };

    for (auto [width, height] : img_sizes) {
      BOOST_LOG_TRIVIAL(info) << "image size: " << width << "x" << height;

      settings_render settings{ width, height, 10, 2 };

      auto start = std::chrono::steady_clock::now();
      QImage img = render(settings, scene);
      auto end = std::chrono::steady_clock::now();
      BOOST_LOG_TRIVIAL(info)
        << "\t\ttime: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
             .count()
        << " ms";

      std::stringstream ss;
      ss << "img_" << cnt << "_fig_" << width << "x" << height << ".jpg";
      img.save(QString::fromStdString(ss.str()));

      start = std::chrono::steady_clock::now();
      QImage img_ll = render_ll(settings, scene);
      end = std::chrono::steady_clock::now();
      BOOST_LOG_TRIVIAL(info)
        << "\t\ttime_ll: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
             .count()
        << " ms";

      std::stringstream ss_ll;
      ss_ll << "img_" << cnt << "_fig_" << width << "x" << height << "_ll"
            << ".jpg";
      img_ll.save(QString::fromStdString(ss_ll.str()));
    }
  }
}
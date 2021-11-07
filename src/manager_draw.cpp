#include "manager_draw.h"

#include <thread>  // thread

void manager_draw::draw(unsigned const width, unsigned const height,
                        std::function<void(QImage)> send_pic) {
  auto th = std::thread(
      [send_pic](int img_w, int img_h) {
        QImage image(img_w, img_h, QImage::Format::Format_ARGB32_Premultiplied);
        image.fill(QColor(255, 255, 255));

        for (int j = img_h - 1; j >= 0; --j) {
          for (int i = 0; i < img_w; ++i) {
            auto r = double(i) / (img_w - 1);
            auto g = double(j) / (img_h - 1);
            auto b = 0.25;

            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);

            image.setPixelColor(i, j, {ir, ig, ib});
          }
        }

        send_pic(image);
      },
      width, height);

  th.detach();
}

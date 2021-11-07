#include "mainwindow.h"

#include "./ui_mainwindow.h"
#include "manager_draw.h"

main_window::main_window(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::main_window) {
  ui->setupUi(this);

  connect(this, &main_window::notify_progress, this,
          &main_window::change_progress);
  connect(this, &main_window::img_rendered, this, &main_window::draw_img);

  scene_ptr = std::make_unique<QGraphicsScene>(ui->gv_canvas);
  ui->gv_canvas->setScene(scene_ptr.get());
  ui->gv_canvas->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->gv_canvas->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void main_window::on_pb_draw_clicked() {
  pd_rend_ptr = std::make_unique<QProgressDialog>("Rendering", "Stop", 0, 100);
  pd_rend_ptr->setMinimumDuration(0);
  pd_rend_ptr->show();
  unsigned width = ui->gv_canvas->width();
  unsigned height = ui->gv_canvas->height();
  manager_draw{}.draw(
      width, height,
      [this](double progress) { emit notify_progress(progress); },
      // TODO: fix thread race ;(
      [this]() -> bool {
        return nullptr == pd_rend_ptr || pd_rend_ptr->wasCanceled();
      },
      [this](QImage img) { emit img_rendered(img); });
}

void main_window::draw_img(QImage image) {
  scene_ptr->clear();
  scene_ptr->addPixmap(QPixmap::fromImage(image));
  pd_rend_ptr->close();
  pd_rend_ptr.reset();
}

void main_window::change_progress(double progress) {
  if (pd_rend_ptr) pd_rend_ptr->setValue(progress);
}

#include "mainwindow.h"

#include "./ui_mainwindow.h"
#include "manager_draw.h"

main_window::main_window(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::main_window) {
  ui->setupUi(this);
  connect(this, &main_window::sss, this, &main_window::uidraw);

  scene_ptr = std::make_unique<QGraphicsScene>(ui->gv_canvas);
  ui->gv_canvas->setScene(scene_ptr.get());
  ui->gv_canvas->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->gv_canvas->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void main_window::on_pb_draw_clicked() {
  unsigned width = ui->gv_canvas->width();
  unsigned height = ui->gv_canvas->height();
  manager_draw{}.draw(width, height, [this](QImage img) { emit sss(img); });
}

void main_window::uidraw(QImage image) {
  scene_ptr->clear();
  scene_ptr->addPixmap(QPixmap::fromImage(image));
}

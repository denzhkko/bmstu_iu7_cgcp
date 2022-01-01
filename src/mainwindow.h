#pragma once

#include "hittable_list.h"
#include <QGraphicsScene>
#include <QMainWindow>
#include <QProgressDialog>
#include <memory> // unique_ptr, shared_ptr

QT_BEGIN_NAMESPACE
namespace Ui {
class main_window;
}
QT_END_NAMESPACE

class main_window : public QMainWindow
{
  Q_OBJECT

public:
  main_window(QWidget* parent = nullptr);

private slots:
  void on_pb_draw_clicked();
  void on_pb_add_object_clicked();
  void on_pb_delete_item_clicked();

  void draw_img(QImage image);
  void change_progress(double progress);

signals:
  void notify_progress(double progress);
  void img_rendered(QImage image);

private:
  void resizeEvent(QResizeEvent* e) override;

  void fillWorldList();

private:
  std::shared_ptr<Ui::main_window> ui;

  std::unique_ptr<QGraphicsScene> scene_ptr;
  std::unique_ptr<QProgressDialog> pd_rend_ptr;

  hittable_list world_;
};

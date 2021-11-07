#pragma once

#include <QGraphicsScene>
#include <QMainWindow>
#include <memory>  // unique_ptr, shared_ptr

QT_BEGIN_NAMESPACE
namespace Ui {
class main_window;
}
QT_END_NAMESPACE

class main_window : public QMainWindow {
  Q_OBJECT

 public:
  main_window(QWidget *parent = nullptr);

 private slots:
  void on_pb_draw_clicked();
  void uidraw(QImage image);

 signals:
  void sss(QImage image);

 private:
  std::shared_ptr<Ui::main_window> ui;

  std::unique_ptr<QGraphicsScene> scene_ptr;
};

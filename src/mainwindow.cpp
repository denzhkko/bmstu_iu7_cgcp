#include "mainwindow.h"

#include <boost/log/trivial.hpp>

#include "./ui_mainwindow.h"
#include "manager_draw.h"
#include "material.h"
#include "sphere.h"
#include "util.h"

main_window::main_window(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::main_window)
{
  ui->setupUi(this);

  connect(
    this, &main_window::notify_progress, this, &main_window::change_progress);
  connect(this, &main_window::img_rendered, this, &main_window::draw_img);

  scene_ptr = std::make_unique<QGraphicsScene>(ui->gv_canvas);
  ui->gv_canvas->setScene(scene_ptr.get());
  ui->gv_canvas->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->gv_canvas->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void
main_window::on_pb_draw_clicked()
{
  pd_rend_ptr = std::make_unique<QProgressDialog>("Rendering", "Stop", 0, 100);
  pd_rend_ptr->setMinimumDuration(0);
  pd_rend_ptr->show();

  unsigned ray_pp = 50;

  if (ui->rb_q_p->isChecked()) {
    ray_pp = 300;
  } else if (ui->rb_q_b->isChecked()) {
    ray_pp = 100;
  } else if (ui->rb_q_q->isChecked()) {
    ray_pp = 10;
  } else {
    BOOST_LOG_TRIVIAL(warning) << "No quality radio button checked";
  }

  // World
  hittable_list world;
  auto tex_checker = make_shared<lambertian>(
    make_shared<checker_texture>(color(0, 0, 0), color(1, 1, 1)));
  auto tex_metall = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
  auto tex_trans = make_shared<dielectric>(1.8);
  auto tex_met_r =
    make_shared<lambertian>(make_shared<solid_color>(color(0.8, 0.6, 0.2)));
  auto tex_met_l = make_shared<metal>(color(0.1, 0.2, 0.5), 0.1);

  world.add(make_shared<sphere>(point3{ 0, -101, 0 }, 100, tex_checker));
  world.add(make_shared<sphere>(point3{ 0, 4, 1 }, 1, tex_trans));
  world.add(make_shared<sphere>(point3{ 2, 0, 0 }, 1, tex_met_r));
  world.add(make_shared<sphere>(point3{ -2, 0, 0 }, 1, tex_met_l));

  color background{ qRed(ui->cp_background->color().rgb()) / 256.0,
                    qGreen(ui->cp_background->color().rgb()) / 256.0,
                    qBlue(ui->cp_background->color().rgb()) / 256.0 };

  scene scene{ background,
               point3{ ui->dsb_pf_x->value(),
                       ui->dsb_pf_y->value(),
                       ui->dsb_pf_z->value() },
               point3{ ui->dsb_pt_x->value(),
                       ui->dsb_pt_y->value(),
                       ui->dsb_pt_z->value() },
               world };

  settings_render rs{ static_cast<unsigned int>(ui->gv_canvas->width()),
                      static_cast<unsigned int>(ui->gv_canvas->height()),
                      ray_pp };

  BOOST_LOG_TRIVIAL(info) << "Canvas: " << rs.width_ << 'x' << rs.height_
                          << "; ray_pp: " << rs.ray_pp_;

  manager_draw{}.draw(
    rs,
    scene,
    [this](double progress) { emit notify_progress(progress); },
    // TODO: fix thread race ;(
    [this]() -> bool {
      return nullptr == pd_rend_ptr || pd_rend_ptr->wasCanceled();
    },
    [this](QImage img) { emit img_rendered(img); });
}

void
main_window::on_pb_add_object_clicked()
{
  auto def_material =
    make_shared<lambertian>(make_shared<solid_color>(color{ 1, 1, 1 }));
  std::shared_ptr<hittable> obj =
    std::make_shared<sphere>(point3{ 0, 0, 0 }, 5, def_material);

  // Choosing figure
  if (ui->rb_no_f_schere->isChecked()) {
    point3 center{ ui->dsb_no_f_s_c_x->value(),
                   ui->dsb_no_f_s_c_y->value(),
                   ui->dsb_no_f_s_c_z->value() };
    double radius = ui->dsb_no_f_s_r->value();

    BOOST_LOG_TRIVIAL(info) << "Schere checked";

    if (ui->rb_no_m_matte->isChecked()) {
      BOOST_LOG_TRIVIAL(info) << "Matte checked";

      if (ui->rb_no_m_m_t_solid->isChecked()) {
        BOOST_LOG_TRIVIAL(info) << "Solid checked";

        color c = to_color(ui->cp_no_m_m_t_s->color());

        auto material = make_shared<lambertian>(make_shared<solid_color>(c));
        obj = std::make_shared<sphere>(center, radius, material);
      } else if (ui->rb_no_m_m_t_checker->isChecked()) {
        BOOST_LOG_TRIVIAL(info) << "Checker checked";

        color c1 = to_color(ui->cp_no_m_m_t_c1->color());
        color c2 = to_color(ui->cp_no_m_m_t_c1->color());

        auto material =
          make_shared<lambertian>(make_shared<checker_texture>(c1, c2));
        obj = std::make_shared<sphere>(center, radius, material);
      } else {
        BOOST_LOG_TRIVIAL(error) << "Texture not checked";
      }
    } else if (ui->rb_no_m_metal->isChecked()) {
      BOOST_LOG_TRIVIAL(info) << "Metall checked";

      color c = to_color(ui->cp_no_m_me_t_s->color());
      auto material = std::make_shared<metal>(c, 0.0);
      obj = std::make_shared<sphere>(center, radius, material);
    } else if (ui->rb_no_m_trans->isChecked()) {
      BOOST_LOG_TRIVIAL(info) << "Transparent checked";
    } else if (ui->rb_no_m_light->isChecked()) {
      BOOST_LOG_TRIVIAL(info) << "Light checked";
    } else {
      BOOST_LOG_TRIVIAL(error) << "Material not checked";
    }

  } else if (ui->rb_no_f_box->isChecked()) {
    BOOST_LOG_TRIVIAL(info) << "Box checked";
  } else {
    BOOST_LOG_TRIVIAL(error) << "Figure not checked";
  }
}

void
main_window::draw_img(QImage image)
{
  scene_ptr->clear();
  scene_ptr->addPixmap(QPixmap::fromImage(image));
  pd_rend_ptr->close();
  pd_rend_ptr.reset();
}

void
main_window::change_progress(double progress)
{
  if (pd_rend_ptr)
    pd_rend_ptr->setValue(progress);
}

void
main_window::resizeEvent(QResizeEvent* e)
{
  QWidget::resizeEvent(e);
  ui->statusbar->showMessage(QString("Canvas: %1x%2")
                               .arg(ui->gv_canvas->width())
                               .arg(ui->gv_canvas->height()));
}

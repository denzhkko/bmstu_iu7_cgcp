#pragma once

#include "color.h"
#include <QColor>

color
to_color(QColor const& qc)
{
  return color{ qRed(qc.rgb()) / 256.0,
                qGreen(qc.rgb()) / 256.0,
                qBlue(qc.rgb()) / 256.0 };
}
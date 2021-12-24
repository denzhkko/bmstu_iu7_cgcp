#include "ColorPicker.h"

#include <QtGui/QPainter>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QStyleOption>

#define PCIKER_ICON_SIZE 48

ColorPicker::ColorPicker(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(PCIKER_ICON_SIZE, PCIKER_ICON_SIZE);

    setWindowTitle("Choose a color");
    mColor = QColor::fromRgbF(1, 1, 1);

    _setBackColor();
}

ColorPicker::ColorPicker(
    QWidget *parent,
    const QColor &color,
    const QString &title) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(PCIKER_ICON_SIZE, PCIKER_ICON_SIZE);

    setWindowTitle(title);
    mColor = color;
    _setBackColor();
}

void ColorPicker::pickColor()
{
    QColor _color = QColorDialog::getColor(mColor, dynamic_cast<QWidget *>(parent()), windowTitle());
    if (_color.isValid())
    {
        mColor = _color;
        _setBackColor();
        colorPicked(mColor);
    }
}

void ColorPicker::mouseReleaseEvent(QMouseEvent *)
{
    pickColor();
}

void ColorPicker::paintEvent(QPaintEvent *)
{
    QStyleOption _option;
    _option.initFrom(this);

    QPainter _painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &_option, &_painter, this);
};

void ColorPicker::_setBackColor()
{
    QString _color = mColor.name(QColor::HexRgb);
    QString _styleSheet = QString("background-color: %1;border: 1px solid black;").arg(_color);
    QWidget::setStyleSheet(_styleSheet);
}

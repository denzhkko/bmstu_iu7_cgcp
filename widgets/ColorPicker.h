#pragma once

#include <QtGui/QColor>
#include <QtGui/QMouseEvent>
#include <QWidget>

// The ColorPicker class provides a widget for picking colors using system color dialog.
class ColorPicker : public QWidget
{
    Q_OBJECT

public:
    ColorPicker(QWidget *parent);
    ColorPicker(QWidget *parent, const QColor &color, const QString &title);

    inline QColor color() const { return mColor; }
    inline QString title() const { return windowTitle(); }

    inline void setColor(const QColor &color)
    {
        mColor = color;
        _setBackColor();
    }
    inline void setTitle(const QString &title) { setWindowTitle(title); }

public slots:
    void pickColor();

signals:
    void colorPicked(const QColor &color);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *) override;

private:
    void _setBackColor();

private:
    QColor mColor;
};

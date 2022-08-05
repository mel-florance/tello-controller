#include "../include/crosshair.h"

Crosshair::Crosshair(QWidget *parent)
    : QWidget{parent},
      color({255, 255, 255, 64}),
      width(300),
      height(300),
      size(25),
      gap(5)
{
    this->repaint();
}

void Crosshair::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(color, 1, Qt::SolidLine));

    auto brush = QBrush();
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    brush.setColor(QColor(255, 0, 0, 127));
    painter.setBrush(brush);

    auto x = round(width / 2);
    auto y = round(height / 2);

    painter.drawLine(x, y + gap, x, y + size + gap); // bottom
    painter.drawLine(x, y - gap, x, y - size - gap); // top
    painter.drawLine(x + gap, y, x + size + gap, y); // right
    painter.drawLine(x - gap, y, x - size - gap, y); // left
}


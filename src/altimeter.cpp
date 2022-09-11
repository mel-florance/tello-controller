#include "../include/altimeter.h"

Altimeter::Altimeter(QWidget *parent)
    : QWidget{parent},
      altitude(0.0),
      color({255, 255, 255, 64})
{
}

void Altimeter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(color, 1, Qt::SolidLine));
    painter.drawLine(0, 0, 0, 280);

    for (int i = 0; i <= 280; i += 28)
        painter.drawLine(2, i, 10, i);

    auto y = Helper::map_value(altitude, {0.0, 20.0}, {280, 0.0});
    painter.drawLine(2, y, 50, y);

    auto m = QFont("monospace");
    m.setPointSize(m.pointSize() * 1.3);
    painter.setFont(m);
    painter.drawText(QPoint(25, y - 10), QString::number(altitude, 'G'));
    m.setPointSize(11);
    painter.setFont(m);
    painter.drawText(QPoint(10, y + 20), "ALT");
}

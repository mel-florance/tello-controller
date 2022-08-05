#include "../include/speedmeter.h"

SpeedMeter::SpeedMeter(QWidget *parent)
    : QWidget{parent},
      speed(0.0),
      color({255, 255, 255, 64})
{
}

void SpeedMeter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(color, 1, Qt::SolidLine));
    auto x = 55.0;

    painter.drawLine(x + 10.0, 0, x + 10.0, 280);

    for (int i = 0; i <= 280; i += 30)
        painter.drawLine(x + 2, i, x + 9, i);

    auto y = Helper::map_value(speed, {0.0, 1.0}, {280.0, 0.0});
    painter.drawLine(20, y, 64, y);

    auto m = QFont("monospace");
    m.setPointSize(m.pointSize() * 1.3);
    painter.setFont(m);
    painter.drawText(QPoint(30, y - 10), QString::number(speed, 'G'));

    m.setPointSize(11);
    painter.setFont(m);
    painter.drawText(QPoint(25, y + 20), "SPD");
}

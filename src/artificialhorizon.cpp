#include "../include/artificialhorizon.h"

ArtificialHorizon::ArtificialHorizon(QWidget *parent)
    : QWidget{parent},
      delta(0.0),
      width(150.0),
      height(150.0),
      max_pitch(50.0),
      radius(width * 0.5),
      yaw_angle(0.0),
      roll_angle(0.0),
      pitch_angle(0.0),
      color({255, 255, 255, 127})
{
}

void ArtificialHorizon::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap));

    auto region = event->region().end();
    auto brush = QBrush();
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    brush.setColor(Qt::black);

    auto x = region->width() * 0.5 - width * 0.5;
    auto y = region->height() * 0.5 - height * 0.5;
    auto outer = 60.0;

    painter.drawArc(x, y, width, height, 0, 360 * 16);
    painter.drawArc(
        x - outer * 0.5,
        y - outer * 0.5,
        width + outer,
        height + outer,
        0, 360 * 16
    );

    auto hw = width * 0.5f;
    auto hh = height * 0.5f;
    for (int i = 0; i <= 35; i++) {
        auto r = qDegreesToRadians(-90.0 + i * 10 + yaw_angle);
        auto l = i % 9 == 0 ? 16 : 0;
        auto gx1 = x + hw + (radius + 2) * cos(r);
        auto gy1 = y + hh + (radius + 2) * sin(r);
        auto gx2 = x + hw + (radius + 12 + l) * cos(r);
        auto gy2 = y + hh + (radius + 12 + l) * sin(r);
        painter.setPen(QPen(color, 1, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(gx1, gy1, gx2, gy2);
    }

    painter.setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap));
    auto alpha = acos(-pitch_angle / max_pitch);
    auto beta = qDegreesToRadians(90.0 - roll_angle);
    auto x1 = x + hw + (radius - 2) * cos(beta + alpha);
    auto y1 = y + hh + (radius - 2) * sin(beta + alpha);
    auto x2 = x + hw + (radius - 2) * cos(beta - alpha);
    auto y2 = y + hh + (radius - 2) * sin(beta - alpha);
    painter.drawLine(x1, y1, x2, y2);
}

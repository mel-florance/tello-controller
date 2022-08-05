#include "../include/gauge.h"

Gauge::Gauge(QWidget *parent)
    : QWidget{parent},
      width(150.0),
      height(150.0),
      value(0.0),
      min(0.0),
      max(100.0),
      color({255, 255, 255, 127}),
      unit(Unit::Type::NONE),
      alerting(true)
{
}

void Gauge::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap));

    if (value < min) value = min;
    if (value > max) value = max;

    auto a = Helper::map_value(value, {min, max}, { qDegreesToRadians(-210), qDegreesToRadians(30)});
    painter.drawArc(2, 2, width-4, height-4, 0, 360 * 16);
    painter.drawLine(width*0.5, height*0.5, width*0.5 + (width*0.5 - 10) * cos(a), height*0.5 + (height*0.5 - 10) * sin(a));

    auto brush = QBrush();
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    brush.setColor(Qt::black);
    painter.setBrush(brush);
    auto knob_radius = 0.1 * width;
    painter.drawEllipse(width*0.5 - (knob_radius*0.5),  width*0.5 - (knob_radius*0.5), knob_radius, knob_radius);

    for (int i = 0; i <= 24; i++) {
        auto r = qDegreesToRadians(-210 + i * 10);
        auto gx1 = width/2 + (width/2 - 10) * cos(r);
        auto gy1 = height/2 + (height/2 - 10) * sin(r);
        auto gx2 = width/2 + (width/2 - 10 + 4) * cos(r);
        auto gy2 = height/2 + (height/2 - 10 + 4) * sin(r);
        painter.setPen(QPen(color, 1, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(gx1, gy1, gx2, gy2);
    }

    QFontMetrics fm = painter.fontMetrics();

    QString str(QString::number(value) + " " + Unit::symbols[unit]);
    auto value_bounds = fm.boundingRect(str);
    int value_width = value_bounds.width();
    int value_height = value_bounds.height();
    painter.drawText(QPoint(width/2 - value_width * 0.5, (height*0.5) - (height*0.25) + value_height*0.5), str);

    if (alerting) {
        QPen pen(Qt::red, 1);
        painter.setPen(pen);
    } else
        painter.setPen(QPen(color, 1, Qt::SolidLine, Qt::RoundCap));

    auto name_bounds = fm.boundingRect(name);
    int name_width = name_bounds.width();
    int name_height = name_bounds.height();
    painter.drawText(QPoint(width/2 - name_width * 0.5, (height*0.5) + (height*0.25) + name_height*0.5), name);
}


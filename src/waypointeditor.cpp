#include "../include/waypointeditor.h"

WaypointEditor::WaypointEditor(QWidget *parent)
    : QWidget{parent},
      label("test", this),
      switch_mode(this)
{
    switch_mode.setText("Hello");
}

void WaypointEditor::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 12, Qt::DashDotLine, Qt::RoundCap));
    painter.drawLine(0, 0, 200, 200);

    label.show();
    switch_mode.show();
}

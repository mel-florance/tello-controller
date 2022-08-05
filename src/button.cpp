#include "../include/button.h"

Button::Button(QWidget *parent, const QString& text)
    : QWidget{parent},
      text(text),
      width(80.0),
      height(30.0),
      border_color({255, 255, 255, 127}),
      background_color({0, 0, 0, 127}),
      text_color({255, 255, 255, 127}),
      highlight(false),
      alerting(false),
      alert_timer(nullptr),
      alert_interval(1000)
{
    this->resize(width, height);
    auto alert_timer_ptr = new QTimer();
    alert_timer.reset(alert_timer_ptr);
}

void Button::startAlerting()
{
    connect(alert_timer.get(), SIGNAL(timeout()), this, SLOT(on_alerttimer()));
    alert_timer->start(alert_interval);
    alerting = true;
}

void Button::stopAlerting()
{
    this->alerting = false;
    this->highlight = false;
    alert_timer->stop();
    disconnect(alert_timer.get(), SIGNAL(timeout()), this, SLOT(on_alerttimer()));
    qDebug() << "stopped";
    this->repaint();
}

void Button::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);


    auto brush = QBrush();
    brush.setColor(background_color);
    painter.setBrush(brush);

    if (!highlight)
        painter.setPen(QPen(border_color, 1, Qt::SolidLine));
    else
       painter.setPen(QPen(Qt::red, 1, Qt::SolidLine));

    painter.drawRect(0, 0, width, height);
    QFontMetrics fm = painter.fontMetrics();
    auto bounds = fm.boundingRect(text);
    painter.drawText(width * 0.5f - bounds.width() * 0.5f, height*0.5f + bounds.height() * 0.5f - 2, text);
}

void Button::on_alerttimer()
{
    highlight = !highlight;
    this->repaint();
}



#ifndef SPEEDMETER_H
#define SPEEDMETER_H

#include <QWidget>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>

#include "../include/helper.h"

class SpeedMeter : public QWidget
{
    Q_OBJECT
public:
    explicit SpeedMeter(QWidget *parent = nullptr);
    inline double getSpeed() const { return speed; }
    inline void setSpeed(double newSpeed) { speed = newSpeed; }

protected:
    void paintEvent(QPaintEvent* event) override;

signals:
private:
    double speed;
    QColor color;
};

#endif // SPEEDMETER_H

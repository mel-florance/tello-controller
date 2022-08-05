#ifndef ALTIMETER_H
#define ALTIMETER_H

#include <QWidget>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>

#include "../include/helper.h"

class Altimeter : public QWidget
{
    Q_OBJECT
public:
    explicit Altimeter(QWidget *parent = nullptr);
    inline double getAltitude() const { return altitude; }
    inline void setAltitude(double newAltitude) { altitude = newAltitude; }

protected:
    void paintEvent(QPaintEvent* event) override;

signals:
private:
    double altitude;
    QColor color;
};

#endif // ALTIMETER_H

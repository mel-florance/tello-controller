#ifndef GAUGE_H
#define GAUGE_H

#include <QWidget>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>

#include "../include/unit.h"
#include "../include/helper.h"

class Gauge : public QWidget
{
    Q_OBJECT
public:
    explicit Gauge(QWidget *parent = nullptr);
    inline double getValue() const { return value; };
    inline void setValue(double newValue) { value = newValue; };
    inline double getMin() const { return min; };
    inline void setMin(double newMin) { min = newMin; };
    inline double getMax() const { return max; };
    inline void setMax(double newMax) { max = newMax; };
    inline Unit::Type getUnit() const { return unit; };
    inline void setUnit(Unit::Type newUnit) { unit = newUnit; };
    inline const QString &getName() const { return name; };
    inline void setName(const QString &newName) { name = newName; };
    inline double getWidth() const { return width; };
    inline void setWidth(double newWidth) { width = newWidth; };
    inline double getHeight() const { return height; };
    inline void setHeight(double newHeight) { height = newHeight; };

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double width;
    double height;
    double value;
    double min;
    double max;
    QColor color;
    Unit::Type unit;
    QString name;
    bool alerting;
};

#endif // GAUGE_H

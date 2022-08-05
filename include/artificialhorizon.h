#ifndef ARTIFICIALHORIZON_H
#define ARTIFICIALHORIZON_H

#include <memory>

#include <QWidget>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>
#include <QLabel>
#include <QPushButton>

class ArtificialHorizon : public QWidget
{
    Q_OBJECT
public:
    explicit ArtificialHorizon(QWidget *parent = nullptr);

    inline void set_yaw_angle(double angle) { this->yaw_angle = angle; };
    inline double get_yaw_angle() { return this->yaw_angle;}
    inline void set_pitch_angle(double angle) { this->pitch_angle = angle; };
    inline double get_pitch_angle() { return this->pitch_angle; };
    inline void set_roll_angle(double angle) { this->roll_angle = angle; };
    inline double get_roll_angle() { return this->roll_angle;}

protected:
    void paintEvent(QPaintEvent* event) override;

signals:

private:
    double delta;
    double width;
    double height;
    double max_pitch;
    double radius;
    double yaw_angle;
    double roll_angle;
    double pitch_angle;
    QColor color;
};

#endif // ARTIFICIALHORIZON_H

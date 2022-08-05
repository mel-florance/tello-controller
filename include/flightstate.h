#ifndef FLIGHTSTATE_H
#define FLIGHTSTATE_H

#include <QVector3D>

struct FlightState {
    QVector3D imu;
    QVector3D speed;
    QVector3D acceleration;
    QVector2D temperature;
    float tof;
    float height;
    int battery;
    int barometer;
    int flight_duration;
};

#endif // FLIGHTSTATE_H

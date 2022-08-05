#ifndef FLIGHTCONTROLLER_H
#define FLIGHTCONTROLLER_H

#include <chrono>
#include <unordered_map>
#include <optional>
#include <cmath>

#include <QObject>
#include <QVector3D>
#include <QQuaternion>
#include <QTimer>

#include "../include/telemetry.h"

using namespace std::chrono;

struct FlightState;
class NetworkController;

class FlightController : public QObject
{
    Q_OBJECT
public:
    explicit FlightController(QObject *parent = nullptr);
    ~FlightController();

    enum class Mode {
        MANUAL,
        FOLLOW
    };

    enum class State {
        LANDING,
        LANDED,
        TAKING_OFF,
        FLYING,
        ERROR
    };

    enum class MoveDirection {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    enum class RotateDirection {
        CLOCKWISE,
        COUNTER_CLOCKWISE
    };

    void update();
    void set_network_controller(std::shared_ptr<NetworkController> controller);
    inline std::shared_ptr<NetworkController> get_network_controller() { return network_controller; };

    void estimate_position();
    Telemetry telemetry;

signals:

public slots:
    void on_flight_state(const FlightState& state);

private:
    Mode mode;
    State state;
    time_point<system_clock> last_command_time;
    std::shared_ptr<NetworkController> network_controller;
    QVector4D position;
};

#endif // FLIGHTCONTROLLER_H

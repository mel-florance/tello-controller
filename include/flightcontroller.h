#ifndef FLIGHTCONTROLLER_H
#define FLIGHTCONTROLLER_H

#include <chrono>
#include <unordered_map>
#include <optional>
#include <QObject>
#include <QVector3D>
#include <QTimer>

#include "../include/metric.h"

using namespace std::chrono;

class NetworkController;
class FlightController : public QObject
{
    Q_OBJECT
public:
    explicit FlightController(QObject *parent = nullptr);

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

    enum class MetricName {
        SPEED,
        MAX_SPEED,
        HEIGHT,
        TIME_OF_FLIGHT,
        PRESSURE,
        BATTERY,
        FLIGHT_DURATION,
        TEMPERATURE,
        WIFI_SIGNAL,
        ANGULAR_RATE,
        ACCELERATION,
        TARGET
    };

    void update();
    inline void set_network_controller(std::shared_ptr<NetworkController> controller) { this->network_controller = controller; };
    inline std::shared_ptr<NetworkController> get_network_controller() { return network_controller; };

    std::optional<Metric> get_metric(MetricName name);
    void set_metric(MetricName name, Metric& metric);
    void set_metric_value(MetricName name, const std::any& value);
    void set_metric_unit(MetricName name, Unit::Type unit);
    void print_metrics();

signals:

private:
    Mode mode;
    State state;
    std::unordered_map<MetricName, Metric> metrics;
    time_point<system_clock> last_command_time;
    std::shared_ptr<NetworkController> network_controller;
};

#endif // FLIGHTCONTROLLER_H

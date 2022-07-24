#include "../include/flightcontroller.h"
#include "../include/networkcontroller.h"
#include "qdebug.h"

FlightController::FlightController(QObject *parent)
    : QObject{parent},
      mode(Mode::MANUAL),
      state(State::LANDED),
      network_controller(nullptr)
{
    Metric speed("speed", 200.0f);
    speed.set_constraint(ConstraintType::GTE, 0.0f);
    speed.set_constraint(ConstraintType::LTE, 100.0f);
    speed.set_unit(Unit::Type::CENTIMETER);
    set_metric(MetricName::SPEED, speed);

    Metric max_speed("max_speed", 0.0f);
    max_speed.set_constraint(ConstraintType::GTE, 0.0f);
    max_speed.set_constraint(ConstraintType::LTE, 100.0f);
    max_speed.set_unit(Unit::Type::CENTIMETER);
    set_metric(MetricName::MAX_SPEED, max_speed);

    Metric height("height", 0.0f);
    height.set_constraint(ConstraintType::GTE, 0.0f);
    height.set_constraint(ConstraintType::LTE, 100.0f);
    height.set_unit(Unit::Type::CENTIMETER);
    set_metric(MetricName::HEIGHT, height);

    Metric time_of_flight("time_of_flight", 0);
    time_of_flight.set_constraint(ConstraintType::GTE, 0);
    time_of_flight.set_constraint(ConstraintType::LTE, 100);
    time_of_flight.set_unit(Unit::Type::SECOND);
    set_metric(MetricName::TIME_OF_FLIGHT, time_of_flight);

    Metric pressure("pressure", 1020.0f);
    pressure.set_constraint(ConstraintType::GTE, 0.0f);
    pressure.set_constraint(ConstraintType::LTE, 2000.0f);
    pressure.set_unit(Unit::Type::MILLIBAR);
    set_metric(MetricName::PRESSURE, pressure);

    auto c = pressure.convert_to(Unit::Type::PSI);

    if (c.has_value()) {
        set_metric_value(MetricName::PRESSURE, c.value());
        set_metric_unit(MetricName::PRESSURE, Unit::Type::PSI);
        qDebug() << "pressure: " << pressure;
    }

    Metric battery("battery", 0);
    battery.set_constraint(ConstraintType::GTE, 0);
    battery.set_constraint(ConstraintType::LTE, 100);
    battery.set_unit(Unit::Type::PERCENT);
    set_metric(MetricName::BATTERY, battery);

    Metric flight_duration("flight_duration", 0);
    flight_duration.set_constraint(ConstraintType::GTE, 0);
    flight_duration.set_constraint(ConstraintType::LTE, 100);
    flight_duration.set_unit(Unit::Type::SECOND);
    set_metric(MetricName::FLIGHT_DURATION, flight_duration);

    Metric temperature("temperature", 0);
    temperature.set_constraint(ConstraintType::GTE, 0);
    temperature.set_constraint(ConstraintType::LTE, 100);
    temperature.set_unit(Unit::Type::CELCIUS);
    set_metric(MetricName::TEMPERATURE, temperature);

    Metric wifi_signal("wifi_signal", 0);
    wifi_signal.set_constraint(ConstraintType::GTE, 0);
    wifi_signal.set_constraint(ConstraintType::LTE, 100);
    wifi_signal.set_unit(Unit::Type::PERCENT);
    set_metric(MetricName::WIFI_SIGNAL, wifi_signal);

    Metric angular_rate("angular_rate", QVector3D(0.0f, 0.0f, 0.0f));
    angular_rate.set_unit(Unit::Type::RADIANS);
    set_metric(MetricName::ANGULAR_RATE, angular_rate);

    Metric acceleration("acceleration", QVector3D(0.0f, 0.0f, 0.0f));
    acceleration.set_unit(Unit::Type::CENTIMETER);
    set_metric(MetricName::ACCELERATION, acceleration);

    Metric target("target", QVector3D(0.0f, 0.0f, 0.0f));
    set_metric(MetricName::TARGET, target);

    print_metrics();
}

void FlightController::update()
{
    auto target_metric = get_metric(MetricName::TARGET);

    // Follow target mode
    if (mode == Mode::FOLLOW
    && state == State::FLYING
    && network_controller
    && target_metric.has_value())
    {
        auto target = target_metric.value().get_value<QVector3D>().value();
        auto now = std::chrono::system_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_command_time);

        if (diff.count() < 1000) return;

        last_command_time = now;

        qDebug() << "Current target: " << target;

        if (target.x() <= -90 || target.x() >= 90) {
            if (target.x() < 0) {
                network_controller->add_command("ccw 20");
                network_controller->flush();
            } else if (target.x() > 0) {
                network_controller->add_command("cw 20");
                network_controller->flush();
            }
        }

        if (target.y() <= -70 || target.y() >= 70) {
            if (target.y() < 0) {
                network_controller->add_command("up 20");
                network_controller->flush();
            } else if (target.y() > 0) {
                network_controller->add_command("down 20");
                network_controller->flush();
            }
        }

        if (target.z() <= 15000 || target.z() >= 30000) {
            if (target.z() < 15000) {
                network_controller->add_command("back 20");
                network_controller->flush();

            } else if (target.z() > 30000) {
                network_controller->add_command("forward 20");
                network_controller->flush();
            }
        }
    }
}

std::optional<Metric> FlightController::get_metric(MetricName name)
{
    auto metric = metrics.find(name);

    if (metric != metrics.end())
       return metric->second;

    return std::nullopt;
}

void FlightController::set_metric(MetricName name, Metric& metric)
{
    metrics.insert(std::make_pair(name, metric));
}

void FlightController::set_metric_value(MetricName name, const std::any &value)
{
    auto it = metrics.find(name);

    if (it != metrics.end())
        it->second.set_value(value);
}

void FlightController::set_metric_unit(MetricName name, Unit::Type unit)
{
    auto it = metrics.find(name);

    if (it != metrics.end())
        it->second.set_unit(unit);
};

void FlightController::print_metrics()
{
   auto debug = qDebug();
   debug.noquote();

   debug << "Metrics:\n";

   for (auto& metric : metrics)
       debug << metric.second << '\n';
}

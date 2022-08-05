#include "../include/networkcontroller.h"
#include "../include/flightcontroller.h"
#include "qapplication.h"
#include "qcoreapplication.h"
#include "qdebug.h"
#include "qguiapplication.h"

FlightController::FlightController(QObject *parent)
    : QObject{parent},
      mode(Mode::MANUAL),
      state(State::LANDED),
      network_controller(nullptr),
      position(QVector4D(0.0f, 0.0f, 0.0f, 0.0f))
{
    telemetry.create_metric("speed", QVector3D(0.0f, 0.0f, 0.0f), Unit::Type::CENTIMETER);
    telemetry.create_metric("max_speed", 0.0f, Unit::Type::CENTIMETER);
    telemetry.create_metric("height", 0.0f, Unit::Type::CENTIMETER);
    telemetry.create_metric("tof", 0.0f, Unit::Type::CENTIMETER);
    telemetry.create_metric("pressure", 0.0f, Unit::Type::MILLIBAR);
    telemetry.create_metric("battery", 0, Unit::Type::PERCENT);
    telemetry.create_metric("flight_duration", 0, Unit::Type::SECOND);
    telemetry.create_metric("temperature",  QVector2D(0.0f, 0.0f), Unit::Type::CELCIUS);
    telemetry.create_metric("wifi_signal", 0, Unit::Type::PERCENT);
    telemetry.create_metric("angular_rate", QVector3D(0.0f, 0.0f, 0.0f), Unit::Type::DEGREES);
    telemetry.create_metric("acceleration", QVector3D(0.0f, 0.0f, 0.0f), Unit::Type::NONE);
    telemetry.create_metric("target", QVector3D(0.0f, 0.0f, 0.0f), Unit::Type::NONE);

    auto telemetry_path = qApp->applicationDirPath() + "/telemetry/";
    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    telemetry.start_record(telemetry_path.toStdString() + std::to_string(timestamp) + ".csv");
}

FlightController::~FlightController()
{
    telemetry.stop_record();
}

void FlightController::update()
{
    auto target = telemetry.get_metric<QVector3D>("target").value();

    // Follow target mode
    if (mode == Mode::FOLLOW
    && state == State::FLYING
    && network_controller
    && !target.isNull())
    {
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

void FlightController::set_network_controller(std::shared_ptr<NetworkController> controller)
{
    this->network_controller = controller;
    connect(controller.get(), SIGNAL(on_controller_state(const FlightState&)), this, SLOT(on_flight_state(const FlightState&)));
}

void FlightController::estimate_position()
{
    auto angular_rate = telemetry.get_metric<QVector3D>("angular_rate").value();
    auto acceleration = telemetry.get_metric<QVector3D>("acceleration").value();
}

void FlightController::on_flight_state(const FlightState& state)
{
    telemetry.record(state);

    telemetry.set_metric_value("speed", state.speed);
    telemetry.set_metric_value("height", state.height);
    telemetry.set_metric_value("tof", state.tof);
    telemetry.set_metric_value("pressure", state.barometer);
    telemetry.set_metric_value("battery", state.battery);
    telemetry.set_metric_value("flight_duration", state.flight_duration);
    telemetry.set_metric_value("temperature", state.temperature);
    telemetry.set_metric_value("angular_rate", state.imu);
    telemetry.set_metric_value("acceleration", state.acceleration);

//    estimate_position();
//    qDebug() << "Position: " << position;
}

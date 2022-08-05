#include "../include/networkcontroller.h"

NetworkController::NetworkController(
    QObject *parent,
    const QHostAddress& address,
    quint16 port
) :
    QObject{parent},
    debug(true),
    address(address),
    port(port),
    state_address(QHostAddress("0.0.0.0")),
    state_port(8890),
    commands({})
{
    socket = new QUdpSocket(this);
    socket->bind(address, port);
    connect(socket, SIGNAL(readyRead()), this, SLOT(ready_read()));

    state_socket = new QUdpSocket(this);
    state_socket->bind(state_address, state_port);
    connect(state_socket, SIGNAL(readyRead()), this, SLOT(state_read()));
}

void NetworkController::send(const QByteArray& data)
{
    socket->writeDatagram(data, address, port);
}

void NetworkController::add_command(const QString& command)
{
    commands.push_back(command);
}

void NetworkController::flush()
{
    if (!commands.empty()) {
        auto next = commands.dequeue();
        last_command = current_command;
        current_command = next;
        send(next.toUtf8());
    }
}

void NetworkController::init()
{
    add_command("command");
    flush();
}

void NetworkController::ready_read()
{
    QHostAddress sender;
    quint16 sender_port;
    QByteArray buffer;

    buffer.resize(socket->pendingDatagramSize());
    socket->readDatagram(buffer.data(), buffer.size(), &sender, &sender_port);

    if (sender != address || sender_port != port) {
        qDebug() << "Error while receiving frame: Unrecognized sender.";
        return;
    }

    if (debug) {
        qDebug() << "Current command: " << current_command;
        qDebug() << "Last command" << last_command;
        qDebug() << "Current buffer: " << buffer;
        qDebug() << "Last buffer: " << last_buffer;
    }

    if (current_command == "command" && buffer == "ok") {
        if (debug)
            qDebug() << "Entering SDK command mode: OK." << buffer;
        emit on_controller_ready();
    }
    else if (current_command == "speed?") {
        auto value = buffer.mid(0, buffer.size() - 2);
        emit on_controller_max_speed(value.toFloat());
        flush();
    }
    else if (current_command == "battery?") {
        auto value = buffer.mid(0, buffer.size() - 2);
        emit on_controller_battery(value.toInt());
        flush();
    }
    else if (current_command == "time?") {
        auto value = buffer.mid(0, buffer.size() - 3);
        emit on_controller_flight_time(value.toInt());
        flush();
    }
    else if (current_command == "height?") {
        auto value = buffer.mid(0, buffer.size() - 4);
        emit on_controller_height(value.toInt() * 10);
        flush();
    }
    else if (current_command == "temp?") {
        auto value = buffer.mid(0, buffer.size() - 3);
        auto parts = value.split('~');
        if (parts.size() == 2) {
            auto v1 = parts.at(0).toInt();
            auto v2 = parts.at(1).toInt();
            emit on_controller_temperature((v1 + v2) / 2);
        }
        else if (parts.size() == 1) {
            emit on_controller_temperature(parts.at(0).toInt());
        }
        flush();
    }
    else if (current_command == "attitude?") {
        auto value = buffer.mid(0, buffer.size() - 2);
        auto parts = value.split(';');

        if (parts.size() == 4) {
            auto pitch = parts.at(0).split(':').at(1);
            auto roll = parts.at(1).split(':').at(1);
            auto yaw = parts.at(2).split(':').at(1);
            QVector3D vector(yaw.toInt(), pitch.toInt(), roll.toInt());
            emit on_controller_imu(vector);
        }
        flush();
    }
    else if (current_command == "baro?") {
        auto value = buffer.mid(0, buffer.size() - 2);
        emit on_controller_barometer(value.toFloat());
        flush();
    }
    else if (current_command == "acceleration?") {
        auto value = buffer.mid(0, buffer.size() - 2);
        auto parts = value.split(';');

        if (parts.size() == 4) {
            auto x = parts.at(0).split(':').at(1);
            auto y = parts.at(1).split(':').at(1);
            auto z = parts.at(2).split(':').at(1);
            QVector3D vector(x.toFloat(), y.toFloat(), z.toFloat());
            emit on_controller_acceleration(vector);
        }
        flush();
    }
    else if (current_command == "tof?") {
        auto value = buffer.mid(0, buffer.size() - 4);
        emit on_controller_time_of_flight(value.toFloat() / 1000.0f);
        flush();
    }
    else if (current_command == "wifi?") {
        auto value = buffer.mid(0, buffer.size() - 2);
        emit on_controller_wifi_snr(value.toInt());
        flush();
    }

    last_buffer = buffer;
}

void NetworkController::state_read()
{
    QHostAddress sender;
    quint16 sender_port;
    QByteArray buffer;

    buffer.resize(state_socket->pendingDatagramSize());
    state_socket->readDatagram(buffer.data(), buffer.size(), &sender, &sender_port);

    if (debug) {
        //qDebug() << "State received: " << buffer;
    }

    // pitch:82;roll:-38;yaw:48;vgx:0;vgy:0;vgz:0;templ:93;temph:94;tof:10;h:0;bat:84;baro:143.70;time:0;agx:942.00;agy:66.00;agz:-123.00;\r\n

    auto parts = buffer.mid(0, buffer.size() - 2).split(';');
    FlightState state;

    for (auto& part : parts) {
        auto metric = part.split(':');

        if (metric.size() != 2)
            continue;

        auto name = metric.at(0);
        auto value = metric.at(1);

        if (name == "yaw")
            state.imu.setX(value.toFloat());
        else if (name == "pitch")
            state.imu.setY(value.toFloat());
        else if (name == "roll")
            state.imu.setZ(value.toFloat());
        else if (name == "vgx")
            state.speed.setX(value.toFloat());
        else if (name == "vgy")
            state.speed.setY(value.toFloat());
        else if (name == "vgz")
            state.speed.setZ(value.toFloat());
        else if (name == "templ")
            state.temperature.setX(value.toInt());
        else if (name == "temph")
            state.temperature.setX(value.toInt());
        else if (name == "tof")
            state.tof = value.toInt();
        else if (name == "h")
            state.height = value.toFloat();
        else if (name == "bat")
            state.battery = value.toInt();
        else if (name == "baro")
            state.barometer = value.toFloat();
        else if (name == "time")
            state.flight_duration = value.toInt();
        else if (name == "agx")
            state.acceleration.setX(value.toFloat());
        else if (name == "agz")
            state.acceleration.setY(value.toFloat());
        else if (name == "agz")
            state.acceleration.setZ(value.toFloat());
    }

    emit on_controller_state(state);
}

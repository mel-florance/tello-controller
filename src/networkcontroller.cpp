#include "../include/networkcontroller.h"

NetworkController::NetworkController(
        QObject *parent,
        const QHostAddress& address,
        quint16 port) :
    QObject{parent},
    debug(true),
    address(address),
    port(port),
    commands({})
{
    socket = new QUdpSocket(this);
    socket->bind(address, port);
    connect(socket, SIGNAL(readyRead()), this, SLOT(ready_read()));
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

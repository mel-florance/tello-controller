#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <chrono>
#include <QObject>
#include <QUdpSocket>
#include <QQueue>
#include <QVector3D>
#include <QVariant>

using namespace std::chrono;

class NetworkController : public QObject
{
    Q_OBJECT
public:
    explicit NetworkController(
        QObject *parent = nullptr,
        const QHostAddress& address = QHostAddress("192.168.10.1"),
        quint16 port = 8889
    );

    void init();
    void flush();
    void send(const QByteArray& data);
    void add_command(const QString& command);

signals:
    void on_controller_ready();
    void on_controller_battery(int percent);
    void on_controller_max_speed(float value);
    void on_controller_flight_time(int seconds);
    void on_controller_height(int centimeters);
    void on_controller_temperature(int degrees);
    void on_controller_imu(const QVector3D& vector);
    void on_controller_barometer(float meters);
    void on_controller_acceleration(const QVector3D& vector);
    void on_controller_time_of_flight(float meters);
    void on_controller_wifi_snr(int ratio);
public slots:
    void ready_read();

private:
    bool debug;
    QHostAddress address;
    quint16 port;
    QUdpSocket* socket;

    QString current_command;
    QString last_command;
    QByteArray last_buffer;
    QQueue<QString> commands;
};

#endif // NETWORKCONTROLLER_H

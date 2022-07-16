#ifndef TELLOCONTROLLER_H
#define TELLOCONTROLLER_H

#include <QObject>
#include <QUdpSocket>
#include <QQueue>
#include <QVector3D>

class TelloController : public QObject
{
    Q_OBJECT
public:
    explicit TelloController(
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
    QHostAddress address;
    quint16 port;
    QUdpSocket* socket;
    QString current_command;
    QQueue<QString> commands;
};

#endif // TELLOCONTROLLER_H

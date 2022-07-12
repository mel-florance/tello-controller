#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QObject>
#include <QTimer>
#include <QProgressBar>

#include "udp_controller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_button_connect_clicked();
    void on_button_takeoff_clicked();
    void on_button_emergency_clicked();
    void on_button_start_video_clicked();

    void on_connected();
    void on_battery(int percent);
    void on_max_speed(float value);
    void on_flight_time(int seconds);
    void on_height(int centimeters);
    void on_temperature(int degrees);
    void on_imu(const QVector3D& vector);
    void on_barometer(float meters);
    void on_acceleration(const QVector3D& vector);
    void on_time_of_flight(float meters);
    void on_wifi_snr(int ratio);

    void on_poll_infos();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<QProgressBar> progress_bar;
    std::unique_ptr<UDPController> controller;
    std::unique_ptr<QTimer> timer;
    bool is_connected;
    bool is_video_started;
    bool is_flying;
};
#endif // MAINWINDOW_H

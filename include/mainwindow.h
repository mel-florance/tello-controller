#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QObject>
#include <QTimer>
#include <QProgressBar>
#include <QThread>
#include <QKeyEvent>

#include "../include/tellocontroller.h"
#include "../include/videoreader.h"
#include "../include/videorecorder.h"

#include <opencv2/core/core.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

Q_DECLARE_METATYPE(cv::Mat);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void start_record();
    void stop_record();

private slots:
    void on_button_connect_clicked();
    void on_button_takeoff_clicked();
    void on_button_emergency_clicked();
    void on_button_start_video_clicked();

    void on_button_move_back_clicked();
    void on_button_move_forward_clicked();
    void on_button_move_left_clicked();
    void on_button_move_right_clicked();
    void on_button_move_up_clicked();
    void on_button_move_down_clicked();
    void on_button_rotate_right_clicked();
    void on_button_rotate_left_clicked();

    void on_groupBox_video_effects_clicked(bool checked);
    void on_edge_detection_radio_clicked();
    void on_face_detection_radio_clicked();
    void on_slider_edge_threshold_ratio_valueChanged(int value);
    void on_slider_edge_min_threshold_valueChanged(int value);
    void on_slider_face_min_neighbors_valueChanged(int value);
    void on_slider_face_scale_factor_valueChanged(int value);

    void on_connected();
    void on_battery(int percent);
    void on_maxspeed(float value);
    void on_flighttime(int seconds);
    void on_height(int centimeters);
    void on_temperature(int degrees);
    void on_imu(const QVector3D& vector);
    void on_barometer(float meters);
    void on_acceleration(const QVector3D& vector);
    void on_timeofflight(float meters);
    void on_wifisnr(int ratio);

    void on_pollinfos();
    void on_videoframe(cv::Mat matrix);
    void on_record_timer();
    void on_alert_timer();

    void on_button_start_recording_clicked();
    void on_splitter_splitterMoved(int pos, int index);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    Ui::MainWindow *ui;
    QThread* video_reader_thread;
    VideoReader* video_reader;
    VideoRecorder* video_recorder;
    QThread* edge_detector_thread;
    QThread* face_detector_thread;
    EdgeDetector* edge_detector;
    FaceDetector* face_detector;
    std::unique_ptr<QProgressBar> progress_bar;
    std::unique_ptr<TelloController> controller;
    std::unique_ptr<QTimer> poll_infos_timer;
    std::unique_ptr<QTimer> record_timer;
    std::unique_ptr<QTimer> alert_timer;
    bool is_connected;
    bool is_video_started;
    bool is_flying;
    bool is_alerting;
    float drone_rotation;

    void enable_flight_controls(bool enable);
    void resize_ui_elements();
};
#endif // MAINWINDOW_H

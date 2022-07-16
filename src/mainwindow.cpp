#include "../include/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
    , progress_bar(nullptr),
      controller(nullptr),
      is_connected(false),
      is_video_started(false),
      is_flying(false),
      drone_rotation(0.0f)
{
    qRegisterMetaType<cv::Mat>();

    ui->setupUi(this);
    ui->statusbar->showMessage("Disconnected");
    ui->button_takeoff->setDisabled(true);
    ui->button_emergency->setDisabled(true);
    ui->button_start_video->setDisabled(true);
    ui->groupBox_video_effects->setDisabled(true);
    enable_flight_controls(false);

    QPixmap pxr(960, 720);
    pxr.fill(Qt::black);
    ui->video->setPixmap(pxr);
    ui->video->setAlignment(Qt::AlignCenter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_connect_clicked()
{
    if (is_connected) {
        ui->input_ip->setDisabled(false);
        ui->input_port->setDisabled(false);
        ui->button_connect->setText("CONNECT");
        ui->button_start_video->setText("Start video");
        ui->button_start_video->setDisabled(true);
        ui->button_takeoff->setText("Take off");
        ui->button_takeoff->setDisabled(true);
        ui->button_emergency->setDisabled(true);
        ui->statusbar->showMessage("Disconnected");
        is_connected = false;
        is_video_started = false;
        is_flying = false;
        progress_bar.reset();
        controller.reset();
        timer.reset();
    }
    else {
        auto address = ui->input_ip->text();
        auto port = ui->input_port->text();

        auto ptr = new TelloController(this, QHostAddress(address), port.toUShort());
        connect(ptr, SIGNAL(on_controller_ready()), this, SLOT(on_connected()));
        connect(ptr, SIGNAL(on_controller_max_speed(float)), this, SLOT(on_maxspeed(float)));
        connect(ptr, SIGNAL(on_controller_battery(int)), this, SLOT(on_battery(int)));
        connect(ptr, SIGNAL(on_controller_flight_time(int)), this, SLOT(on_flighttime(int)));
        connect(ptr, SIGNAL(on_controller_height(int)), this, SLOT(on_height(int)));
        connect(ptr, SIGNAL(on_controller_temperature(int)), this, SLOT(on_temperature(int)));
        connect(ptr, SIGNAL(on_controller_imu(const QVector3D&)), this, SLOT(on_imu(const QVector3D&)));
        connect(ptr, SIGNAL(on_controller_barometer(float)), this, SLOT(on_barometer(float)));
        connect(ptr, SIGNAL(on_controller_acceleration(const QVector3D&)), this, SLOT(on_acceleration(const QVector3D&)));
        connect(ptr, SIGNAL(on_controller_time_of_flight(float)), this, SLOT(on_timeofflight(float)));
        connect(ptr, SIGNAL(on_controller_wifi_snr(int)), this, SLOT(on_wifisnr(int)));
        controller.reset(ptr);
        controller->init();

        auto progress_bar_ptr = new QProgressBar(this);
        progress_bar_ptr->setVisible(false);
        progress_bar_ptr->setRange(0, 100);
        progress_bar_ptr->setValue(0);
        progress_bar_ptr->setAlignment(Qt::AlignCenter);
        progress_bar_ptr->setMaximumSize(180, 19);
        progress_bar_ptr->setTextVisible(true);
        progress_bar_ptr->setFormat("Battery (0 %)");
        ui->statusbar->addPermanentWidget(progress_bar_ptr, 2);
        progress_bar.reset(progress_bar_ptr);

        ui->statusbar->showMessage("Connecting...");
    }
}

void MainWindow::on_connected()
{
    progress_bar->setVisible(true);
    ui->input_ip->setDisabled(true);
    ui->input_port->setDisabled(true);
    ui->button_connect->setText("DISCONNECT");
    ui->statusbar->showMessage("Connected");
    ui->button_takeoff->setDisabled(false);
    ui->button_emergency->setDisabled(false);
    ui->button_start_video->setDisabled(false);
    is_connected = true;
    on_pollinfos();

    auto ptr = new QTimer(this);
    connect(ptr, SIGNAL(timeout()), this, SLOT(on_pollinfos()));
    timer.reset(ptr);
//    timer->start(3000);
}

void MainWindow::on_battery(int percent) {
    progress_bar->setValue(percent);
    progress_bar->setFormat("Battery (" + QString::number(percent) + " %)");
}

void MainWindow::on_maxspeed(float value) {
    ui->drone_speed->setText(QString::number(value) + " cm /s");
}

void MainWindow::on_flighttime(int seconds) {
    ui->drone_flight_time->setText(QString::number(seconds) + " s");
}

void MainWindow::on_height(int centimeters) {
    ui->drone_height->setText(QString::number(centimeters) + " cm");
}

void MainWindow::on_temperature(int degrees) {
    ui->drone_temperature->setText(QString::number(degrees) + "°C");
}

void MainWindow::on_imu(const QVector3D &v) {
    ui->drone_imu->setText(QString("%1, %2, %3").arg(v.x()).arg(v.y()).arg(v.z()));
}

void MainWindow::on_barometer(float meters) {
    ui->drone_pressure->setText(QString::number(meters) + " m");
}

void MainWindow::on_acceleration(const QVector3D &v) {
    ui->drone_acceleration->setText(QString("%1, %2, %3").arg(v.x()).arg(v.y()).arg(v.z()));
}

void MainWindow::on_timeofflight(float meters) {
    ui->drone_tof->setText(QString::number(meters) + " m");
}

void MainWindow::on_wifisnr(int ratio) {
    ui->drone_wifi->setText(QString::number(ratio));
}

void MainWindow::on_pollinfos()
{
    if (is_connected) {
        const QList commands {
             "speed?",
             "battery?",
             "time?",
             "height?",
             "temp?",
             "attitude?",
             "baro?",
             "acceleration?",
             "tof?",
             "wifi?"
        };

        for (auto& command : commands)
            controller->add_command(command);

        controller->flush();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!is_connected || !is_flying)
        return;

//    if(event->key() == Qt::Key_Z)
//    {
//        qDebug() << "z pressed";
//        controller->add_command("forward 20");
//        controller->flush();
//    }

//    if(event->key() == Qt::Key_S)
//    {
//        qDebug() << "s pressed";
//        controller->add_command("back 20");
//        controller->flush();
//    }

//    if(event->key() == Qt::Key_Q)
//    {
//        qDebug() << "q pressed";
//        controller->add_command("left 20");
//        controller->flush();
//    }

//    if(event->key() == Qt::Key_D)
//    {
//        qDebug() << "d pressed";
//        controller->add_command("right 20");
//        controller->flush();
//    }

//    if(event->key() == Qt::Key_A)
//    {
//        qDebug() << "a pressed";
//        controller->add_command("down 20");
//        controller->flush();
//    }

//    if(event->key() == Qt::Key_E)
//    {
//        qDebug() << "e pressed";
//        controller->add_command("up 20");
//        controller->flush();
//    }
}

void MainWindow::enable_flight_controls(bool enable)
{
    ui->button_move_back->setDisabled(!enable);
    ui->button_move_forward->setDisabled(!enable);
    ui->button_move_left->setDisabled(!enable);
    ui->button_move_right->setDisabled(!enable);
    ui->button_move_up->setDisabled(!enable);
    ui->button_move_down->setDisabled(!enable);
    ui->button_rotate_left->setDisabled(!enable);
    ui->button_rotate_right->setDisabled(!enable);
}

void MainWindow::on_button_takeoff_clicked()
{
    if (is_connected) {
        if (is_flying) {
            is_flying = false;
            controller->add_command("land");
            controller->flush();
            ui->button_takeoff->setText("Take off");
            enable_flight_controls(false);

        } else {
            is_flying = true;
            controller->add_command("takeoff");
            controller->flush();
            ui->button_takeoff->setText("Land");
            enable_flight_controls(true);
        }
    }
}

void MainWindow::on_button_emergency_clicked()
{
    if (is_connected) {
        controller->add_command("emergency");
        controller->flush();
    }
}


void MainWindow::on_button_start_video_clicked()
{
    if (is_connected) {
        if (is_video_started) {
            is_video_started = false;
            controller->add_command("streamoff");
            controller->flush();
            ui->button_start_video->setText("Start video");
            ui->groupBox_video_effects->setDisabled(true);
            video_reader_thread->terminate();
            QPixmap pxr(ui->video->width(), ui->video->height());
            pxr.fill(Qt::black);
            ui->video->setPixmap(pxr);
        } else {
            is_video_started = true;
            controller->add_command("streamon");
            controller->flush();
            ui->button_start_video->setText("Stop video");
            ui->groupBox_video_effects->setDisabled(false);

            video_reader_thread = new QThread();
            video_reader = new VideoReader();
            video_reader->moveToThread(video_reader_thread);
            connect(video_reader_thread, &QThread::started, video_reader, &VideoReader::process);
            connect(video_reader, SIGNAL(decoded_frame(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
            connect(video_reader_thread, &QThread::finished, video_reader_thread, &QThread::deleteLater);
            video_reader_thread->start();

            face_detector_thread = new QThread();
            face_detector = new FaceDetector();
            connect(video_reader, SIGNAL(decoded_frame(cv::Mat)), face_detector, SLOT(detect(cv::Mat)));
            //connect(face_detector, SIGNAL(faceframe(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
            face_detector->moveToThread(face_detector_thread);
            face_detector_thread->start();

            edge_detector_thread = new QThread();
            edge_detector = new EdgeDetector();
            connect(video_reader, SIGNAL(decoded_frame(cv::Mat)), edge_detector, SLOT(detect(cv::Mat)));
            //connect(edge_detector, SIGNAL(edgeframe(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
            edge_detector->moveToThread(edge_detector_thread);
            edge_detector_thread->start();
        }
    }
}

void MainWindow::on_videoframe(cv::Mat matrix)
{
    auto width = ui->video->width();
    auto height = ui->video->height();
    if (width > 960) width = 960;
    if (height > 720) height = 720;
    QImage pixels((uchar*)matrix.data, matrix.cols, matrix.rows, matrix.step, QImage::Format_RGB888);
    auto image = QPixmap::fromImage(pixels.rgbSwapped());
    ui->video->setPixmap(image.scaled(width, height, Qt::KeepAspectRatio));
}

void MainWindow::on_button_move_up_clicked()
{
    if (is_connected && is_flying) {
        controller->add_command("up 20");
        controller->flush();
    }
}


void MainWindow::on_button_move_left_clicked()
{
    if (is_connected && is_flying) {
        controller->add_command("left 20");
        controller->flush();
    }
}


void MainWindow::on_button_move_forward_clicked()
{
    if (is_connected && is_flying) {
        controller->add_command("forward 20");
        controller->flush();
    }
}


void MainWindow::on_button_rotate_left_clicked()
{
    if (is_connected && is_flying) {
        controller->add_command("ccw 20");
        controller->flush();
        drone_rotation -= 20;
        ui->text_drone_rotation->setText(QString::number(drone_rotation) + "°");
    }
}


void MainWindow::on_button_move_right_clicked()
{
    if (is_connected && is_flying) {
        controller->add_command("right 20");
        controller->flush();
    }
}


void MainWindow::on_button_rotate_right_clicked()
{
    if (is_connected && is_flying) {
        controller->add_command("cw 20");
        controller->flush();
        drone_rotation += 20;
        ui->text_drone_rotation->setText(QString::number(drone_rotation) + "°");
    }
}


void MainWindow::on_button_move_back_clicked()
{
    if (is_connected && is_flying) {
        controller->add_command("back 20");
        controller->flush();
    }
}


void MainWindow::on_button_move_down_clicked()
{
    if (is_connected && is_flying) {
        controller->add_command("down 20");
        controller->flush();
    }
}

void MainWindow::on_slider_edge_threshold_ratio_valueChanged(int value)
{
    if (edge_detector) {
        edge_detector->ratio = value;
        ui->edge_threshold_ratio->setText(QString::number(value));
    }
}

void MainWindow::on_slider_edge_min_threshold_valueChanged(int value)
{
    if (edge_detector) {
        edge_detector->min_threshold = value;
        ui->edge_min_threshold->setText(QString::number(value));
    }
}


void MainWindow::on_slider_face_min_neighbors_valueChanged(int value)
{
    if (face_detector) {
        face_detector->min_neighbors = value;
        ui->face_min_neighbors->setText(QString::number(value));
    }
}


void MainWindow::on_slider_face_scale_factor_valueChanged(int value)
{
    if (face_detector) {
        if (value <= 0)
            value = 1;
        face_detector->scale_factor = value;
        ui->face_scale_factor->setText(QString::number(value));
    }
}

void MainWindow::on_groupBox_video_effects_clicked(bool checked)
{
    if (!is_video_started)
        return;

    if(checked) {
        if (ui->face_detection_radio->isChecked()) {
            disconnect(video_reader, SIGNAL(decoded_frame(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
            disconnect(edge_detector, SIGNAL(edgeframe(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
            connect(face_detector, SIGNAL(faceframe(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
            face_detector->enabled = true;
            edge_detector->enabled = false;
        } else if (ui->edge_detection_radio->isChecked()) {
            disconnect(video_reader, SIGNAL(decoded_frame(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
            disconnect(face_detector, SIGNAL(faceframe(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
            connect(edge_detector, SIGNAL(edgeframe(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
            face_detector->enabled = false;
            edge_detector->enabled = true;
        }
    } else {
        edge_detector->enabled = false;
        face_detector->enabled = false;
        connect(video_reader, SIGNAL(decoded_frame(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
    }
}

void MainWindow::on_edge_detection_radio_clicked()
{
    disconnect(video_reader, SIGNAL(decoded_frame(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
    disconnect(face_detector, SIGNAL(faceframe(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
    connect(edge_detector, SIGNAL(edgeframe(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
    face_detector->enabled = false;
    edge_detector->enabled = true;
}

void MainWindow::on_face_detection_radio_clicked()
{
    disconnect(video_reader, SIGNAL(decoded_frame(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
    disconnect(edge_detector, SIGNAL(edgeframe(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
    connect(face_detector, SIGNAL(faceframe(cv::Mat)), this, SLOT(on_videoframe(cv::Mat)));
    face_detector->enabled = true;
    edge_detector->enabled = false;
}


#include "../include/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
    , battery_progress_bar(nullptr),
      wifi_progress_bar(nullptr),
      network_controller(nullptr),
      flight_controller(nullptr),
      artificial_horizon(nullptr),
      altimeter(nullptr),
      is_connected(false),
      is_video_started(false),
      is_flying(false),
      is_alerting(false),
      drone_rotation(0.0f),
      follow_target(false)
{
    qRegisterMetaType<cv::Mat>();

    ui->setupUi(this);
    ui->statusbar->showMessage("Disconnected");
    ui->button_takeoff->setDisabled(true);
    ui->button_emergency->setDisabled(true);
    ui->button_start_video->setDisabled(true);
    ui->groupBox_video_effects->setDisabled(true);
    ui->button_start_recording->setDisabled(true);
    ui->recording_dot->setVisible(false);
    ui->recording_text->setVisible(false);
    ui->alert_message->setVisible(false);
    ui->button_remove_waypoint->setDisabled(true);
    ui->statusbar->setStyleSheet("background-color: rgb(220, 220, 220);");

    flight_controller = std::make_unique<FlightController>(this);
    enable_flight_controls(false);

    QPixmap pxr(960, 720);
    pxr.fill(Qt::black);
    ui->video->setPixmap(pxr);
    ui->video->setAlignment(Qt::AlignCenter);

    artificial_horizon = std::make_unique<ArtificialHorizon>(ui->video);
    artificial_horizon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    artificial_horizon->resize(302, 302);

    altimeter = std::make_unique<Altimeter>(ui->video);
    altimeter->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    altimeter->resize(302, 302);

    speedmeter = std::make_unique<SpeedMeter>(ui->video);
    speedmeter->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    speedmeter->resize(302, 302);

    crosshair = std::make_unique<Crosshair>(ui->video);
    crosshair->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    crosshair->resize(302, 302);

    temperature_gauge = std::make_unique<Gauge>(ui->video);
    temperature_gauge->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    temperature_gauge->resize(100, 100);
    temperature_gauge->setWidth(100.0);
    temperature_gauge->setHeight(100.0);
    temperature_gauge->setMax(95.0);
    temperature_gauge->setUnit(Unit::Type::CELCIUS);
    temperature_gauge->setName("TEMP.");

    battery_gauge = std::make_unique<Gauge>(ui->video);
    battery_gauge->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    battery_gauge->resize(100, 100);
    battery_gauge->setWidth(100.0);
    battery_gauge->setHeight(100.0);
    battery_gauge->setUnit(Unit::Type::PERCENT);
    battery_gauge->setName("BATTERY");

    pressure_gauge = std::make_unique<Gauge>(ui->video);
    pressure_gauge->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    pressure_gauge->resize(100, 100);
    pressure_gauge->setUnit(Unit::Type::MILLIBAR);
    pressure_gauge->setName("PRESSURE");
    pressure_gauge->setWidth(100.0);
    pressure_gauge->setHeight(100.0);
    pressure_gauge->setMin(100.0);
    pressure_gauge->setMax(1050.0);

    wifi_gauge = std::make_unique<Gauge>(ui->video);
    wifi_gauge->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    wifi_gauge->resize(100, 100);
    wifi_gauge->setUnit(Unit::Type::PERCENT);
    wifi_gauge->setName("WIFI");
    wifi_gauge->setWidth(100.0);
    wifi_gauge->setHeight(100.0);

    landing_button = std::make_unique<Button>(ui->video, "LANDING");
    power_button = std::make_unique<Button>(ui->video, "POWER");
    signal_button = std::make_unique<Button>(ui->video, "SIGNAL");
    temp_button = std::make_unique<Button>(ui->video, "TEMP.");
    pressure_button = std::make_unique<Button>(ui->video, "PRESSURE");
    inair_button = std::make_unique<Button>(ui->video, "IN AIR");
    height_button = std::make_unique<Button>(ui->video, "HEIGHT");
    recording_button = std::make_unique<Button>(ui->video, "RECORDING");
    auto_pilot_button = std::make_unique<Button>(ui->video, "AUTO-PILOT");
    lighting_button = std::make_unique<Button>(ui->video, "LIGHTING");

    video_recorder = new VideoRecorder();
    auto waypoint_editor_ptr = new WaypointEditor();
    waypoint_editor.reset(waypoint_editor_ptr);
    ui->tabWidget->addTab(waypoint_editor.get(), "Waypoints");

    auto battery_progress_bar_ptr = new QProgressBar(this);
    battery_progress_bar_ptr->setDisabled(true);
    battery_progress_bar_ptr->setRange(0, 100);
    battery_progress_bar_ptr->setValue(0);
    battery_progress_bar_ptr->setAlignment(Qt::AlignVCenter);
    battery_progress_bar_ptr->setMaximumSize(180, 19);
    battery_progress_bar_ptr->setTextVisible(true);
    battery_progress_bar_ptr->setFormat("Battery (0 %)");
    ui->statusbar->addPermanentWidget(battery_progress_bar_ptr, 2);
    battery_progress_bar.reset(battery_progress_bar_ptr);

    auto wifi_progress_bar_ptr = new QProgressBar(this);
    wifi_progress_bar_ptr->setDisabled(true);
    wifi_progress_bar_ptr->setRange(0, 100);
    wifi_progress_bar_ptr->setValue(0);
    wifi_progress_bar_ptr->setAlignment(Qt::AlignVCenter);
    wifi_progress_bar_ptr->setMaximumSize(180, 19);
    wifi_progress_bar_ptr->setTextVisible(true);
    wifi_progress_bar_ptr->setFormat("Wifi (0 %)");
    ui->statusbar->addPermanentWidget(wifi_progress_bar_ptr, 2);
    wifi_progress_bar.reset(wifi_progress_bar_ptr);

    auto temperature_progress_bar_ptr = new QProgressBar(this);
    temperature_progress_bar_ptr->setDisabled(true);
    temperature_progress_bar_ptr->setRange(0, 100);
    temperature_progress_bar_ptr->setValue(0);
    temperature_progress_bar_ptr->setAlignment(Qt::AlignVCenter);
    temperature_progress_bar_ptr->setMaximumSize(180, 19);
    temperature_progress_bar_ptr->setTextVisible(true);
    temperature_progress_bar_ptr->setFormat("Temperature (0°C)");
    ui->statusbar->addPermanentWidget(temperature_progress_bar_ptr, 2);
    temperature_progress_bar.reset(temperature_progress_bar_ptr);

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
        battery_progress_bar->setDisabled(true);
        battery_progress_bar->setValue(0);
        battery_progress_bar->setFormat("Battery (0 %)");
        wifi_progress_bar->setDisabled(true);
        wifi_progress_bar->setValue(0);
        wifi_progress_bar->setFormat("Wifi (0 %)");
        ui->statusbar->showMessage("Disconnected");
        is_connected = false;
        is_video_started = false;
        is_flying = false;
        network_controller.reset();
        poll_infos_timer.reset();
    }
    else {
        auto address = ui->input_ip->text();
        auto port = ui->input_port->text();
       landing_button->startAlerting();
        auto ptr = new NetworkController(this, QHostAddress(address), port.toUShort());
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
        connect(ptr, SIGNAL(on_controller_state(const FlightState&)), this, SLOT(on_flightstate(const FlightState&)));
        network_controller.reset(ptr);
        network_controller->init();
        flight_controller->set_network_controller(network_controller);

        ui->statusbar->showMessage("Connecting...");
    }
}

void MainWindow::on_connected()
{
    ui->input_ip->setDisabled(true);
    ui->input_port->setDisabled(true);
    ui->button_connect->setText("DISCONNECT");
    ui->statusbar->showMessage("Connected");
    ui->button_takeoff->setDisabled(false);
    ui->button_emergency->setDisabled(false);
    ui->button_start_video->setDisabled(false);
    battery_progress_bar->setVisible(true);
    battery_progress_bar->setDisabled(false);
    wifi_progress_bar->setVisible(true);
    wifi_progress_bar->setDisabled(false);
    is_connected = true;
    on_pollinfos();

    auto ptr = new QTimer(this);
    connect(ptr, SIGNAL(timeout()), this, SLOT(on_pollinfos()));
    poll_infos_timer.reset(ptr);
    //poll_infos_timer->start(2500);
}

void MainWindow::on_battery(int percent)
{
    if (percent < 10 && !is_alerting) {
        auto ptr = new QTimer(this);
        connect(ptr, SIGNAL(timeout()), this, SLOT(on_alerttimer()));
        alert_timer.reset(ptr);
        alert_timer->start(1000);
        is_alerting = true;
        ui->alert_message->setText("BATTERY ALERT\n" + QString::number(percent) + " %\nFIND A PLACE TO LAND");
    }
    else if (is_alerting) {
        ui->alert_message->setText("BATTERY ALERT\n" + QString::number(percent) + " %\nFIND A PLACE TO LAND");
    }

    battery_progress_bar->setValue(percent);
    battery_progress_bar->setFormat("Battery (" + QString::number(percent) + " %)");
}

void MainWindow::on_speed(float value) {
    ui->drone_speed->setText(QString::number(value) + " cm /s");
}

void MainWindow::on_maxspeed(float value) {
    ui->drone_max_speed->setText(QString::number(value) + " cm /s");
}

void MainWindow::on_flighttime(int seconds) {
    ui->drone_flight_time->setText(QString::number(seconds) + " s");
}

void MainWindow::on_height(int centimeters) {
    ui->drone_height->setText(QString::number(centimeters) + " cm");
}

void MainWindow::on_temperature(int degrees) {
    if (degrees > 90 && !is_alerting) {
        auto ptr = new QTimer(this);
        connect(ptr, SIGNAL(timeout()), this, SLOT(on_alert_timer()));
        alert_timer.reset(ptr);
        alert_timer->start(1000);
        is_alerting = true;
        ui->alert_message->setText("TEMPERATURE ALERT\n" + QString::number(degrees) + "°C\nFIND A PLACE TO LAND");
    }

    temperature_progress_bar->setValue(degrees);
    temperature_progress_bar->setFormat("Temperature (" + QString::number(degrees) + "°C)");
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
    wifi_progress_bar->setValue(ratio);
    wifi_progress_bar->setFormat("Wifi (" + QString::number(ratio) + " %)");
    wifi_gauge->setValue((double)ratio);
    wifi_gauge->repaint();
}

void MainWindow::on_pollinfos()
{
    if (is_connected) {
        const QList commands {
             "temp?",
             "battery?",
             "wifi?"
//             "speed?",
//             "time?",
//             "height?",
//             "attitude?",
//             "baro?",
//             "acceleration?",
//             "tof?",

        };

        for (auto& command : commands)
            network_controller->add_command(command);

        network_controller->flush();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!is_connected || !is_flying)
        return;

//    if(event->key() == Qt::Key_Z)
//    {
//        qDebug() << "z pressed";
//        network_controller->add_command("forward 20");
//        network_controller->flush();
//    }

//    if(event->key() == Qt::Key_S)
//    {
//        qDebug() << "s pressed";
//        network_controller->add_command("back 20");
//        network_controller->flush();
//    }

//    if(event->key() == Qt::Key_Q)
//    {
//        qDebug() << "q pressed";
//        network_controller->add_command("left 20");
//        network_controller->flush();
//    }

//    if(event->key() == Qt::Key_D)
//    {
//        qDebug() << "d pressed";
//        network_controller->add_command("right 20");
//        network_controller->flush();
//    }

//    if(event->key() == Qt::Key_A)
//    {
//        qDebug() << "a pressed";
//        network_controller->add_command("down 20");
//        network_controller->flush();
//    }

//    if(event->key() == Qt::Key_E)
//    {
//        qDebug() << "e pressed";
//        network_controller->add_command("up 20");
//        network_controller->flush();
    //    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    resize_ui_elements();
}

void MainWindow::showEvent(QShowEvent *event)
{
    ui->splitter->setStretchFactor(1, 1);

    QTimer::singleShot(10, [=, this]{
        resize_ui_elements();
    });
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

void MainWindow::resize_ui_elements()
{
    ui->video->setGeometry(QRect{0, 0,
        ui->tabWidget->geometry().width(),
        ui->tabWidget->geometry().height()
    });

    ui->recording_text->setGeometry(QRect{
        ui->tabWidget->geometry().width() - ui->recording_text->geometry().width() - 10,
        10,
        ui->recording_text->geometry().width(),
        ui->recording_text->geometry().height()
    });

    ui->recording_dot->setGeometry(QRect{
        ui->tabWidget->geometry().width() - ui->recording_text->geometry().width() - ui->recording_dot->geometry().width() - 15,
        5,
        ui->recording_dot->geometry().width(),
        ui->recording_dot->geometry().height()
    });

    ui->alert_message->setGeometry(QRect{
        (ui->tabWidget->geometry().width()/2) - 350/2,
        (ui->tabWidget->geometry().height()/2) - 140/2,
        350,
        140
    });

    if (artificial_horizon)
        artificial_horizon->move(ui->tabWidget->geometry().width() * 0.5f - 150.0f, ui->tabWidget->geometry().height() - 300.0f);
    if (altimeter)
        altimeter->move(ui->tabWidget->geometry().width() * 0.5f - 300.0f, ui->tabWidget->geometry().height() * 0.5f - 180.0f);
    if (speedmeter)
        speedmeter->move(ui->tabWidget->geometry().width() * 0.5f + 232.0f, ui->tabWidget->geometry().height() * 0.5f - 180.0f);
    if (crosshair)
        crosshair->move(ui->tabWidget->geometry().width() * 0.5f - 150.0f, ui->tabWidget->geometry().height() * 0.5f - 180.0f);
    if (temperature_gauge)
        temperature_gauge->move(artificial_horizon->geometry().x() - 60.0f, artificial_horizon->geometry().y() + 140.0f);
    if (battery_gauge)
        battery_gauge->move(artificial_horizon->geometry().x() + 260.0f, artificial_horizon->geometry().y() + 140.0f);
    if (pressure_gauge)
        pressure_gauge->move(artificial_horizon->geometry().x() - 60.0f, artificial_horizon->geometry().y() + 30.0f);
    if (wifi_gauge)
        wifi_gauge->move(artificial_horizon->geometry().x() + 260.0f, artificial_horizon->geometry().y() + 30.0f);
    if (landing_button)
        landing_button->move(artificial_horizon->geometry().x() - 150.0f, artificial_horizon->geometry().y() + 40.0f);
    if (power_button)
        power_button->move(artificial_horizon->geometry().x() - 150.0f, artificial_horizon->geometry().y() + 80.0f);
    if (signal_button)
        signal_button->move(artificial_horizon->geometry().x() - 150.0f, artificial_horizon->geometry().y() + 120.0f);
    if (temp_button)
        temp_button->move(artificial_horizon->geometry().x() - 150.0f, artificial_horizon->geometry().y() + 160.0f);
    if (pressure_button)
        pressure_button->move(artificial_horizon->geometry().x() - 150.0f, artificial_horizon->geometry().y() + 200.0f);
    if (inair_button)
        inair_button->move(artificial_horizon->geometry().x() + 370.0f, artificial_horizon->geometry().y() + 40.0f);
    if (height_button)
        height_button->move(artificial_horizon->geometry().x() + 370.0f, artificial_horizon->geometry().y() + 80.0f);
    if (recording_button)
        recording_button->move(artificial_horizon->geometry().x() + 370.0f, artificial_horizon->geometry().y() + 120.0f);
    if (auto_pilot_button)
        auto_pilot_button->move(artificial_horizon->geometry().x() + 370.0f, artificial_horizon->geometry().y() + 160.0f);
    if (lighting_button)
        lighting_button->move(artificial_horizon->geometry().x() + 370.0f, artificial_horizon->geometry().y() + 200.0f);

    ui->video->repaint();
}

void MainWindow::on_button_takeoff_clicked()
{
    if (is_connected) {
        if (is_flying) {
            is_flying = false;
            network_controller->add_command("land");
            network_controller->flush();
            ui->button_takeoff->setText("Take off");
            enable_flight_controls(false);

        } else {
            is_flying = true;
            network_controller->add_command("takeoff");
            network_controller->flush();
            ui->button_takeoff->setText("Land");
            enable_flight_controls(true);
        }
    }
}

void MainWindow::on_button_emergency_clicked()
{
    if (is_connected) {
        network_controller->add_command("emergency");
        network_controller->flush();
    }
}


void MainWindow::on_button_start_video_clicked()
{
    if (is_connected) {
        if (is_video_started) {
            is_video_started = false;
            network_controller->add_command("streamoff");
            network_controller->flush();
            ui->button_start_video->setText("Start video");
            ui->groupBox_video_effects->setDisabled(true);
            ui->button_start_recording->setDisabled(true);

            disconnect(video_reader, SIGNAL(decoded_frame(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
            disconnect(edge_detector, SIGNAL(edgeframe(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
            disconnect(face_detector, SIGNAL(faceframe(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
            video_reader->stop();
            video_reader_thread->quit();
            face_detector_thread->quit();
            edge_detector_thread->quit();

            QPixmap pxr(ui->video->width(), ui->video->height());
            pxr.fill(Qt::black);
            ui->video->setPixmap(pxr);
        } else {
            is_video_started = true;
            network_controller->add_command("streamon");
            network_controller->flush();
            ui->button_start_video->setText("Stop video");
            ui->groupBox_video_effects->setDisabled(false);
            ui->button_start_recording->setDisabled(false);

            video_reader_thread = new QThread();
            video_reader = new VideoReader();
            video_reader->moveToThread(video_reader_thread);
            connect(video_reader_thread, &QThread::started, video_reader, &VideoReader::process);
            connect(video_reader, SIGNAL(decoded_frame(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
            connect(video_reader_thread, &QThread::finished, video_reader_thread, &QThread::deleteLater);
            video_reader_thread->start();

            face_detector_thread = new QThread();
            face_detector = new FaceDetector();
            connect(face_detector, SIGNAL(faceoffset(QVector3D&)), this, SLOT(on_faceoffset(QVector3D&)));
            face_detector->moveToThread(face_detector_thread);
            face_detector_thread->start();

            edge_detector_thread = new QThread();
            edge_detector = new EdgeDetector();
            edge_detector->moveToThread(edge_detector_thread);
            edge_detector_thread->start();
        }
    }
}

void MainWindow::on_videoframe(cv::Mat& matrix)
{
    if (video_recorder->is_recording()) {
        video_recorder->write(matrix);
    }

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
        network_controller->add_command("up 20");
        network_controller->flush();
    }
}

void MainWindow::on_button_move_left_clicked()
{
    if (is_connected && is_flying) {
        network_controller->add_command("left 20");
        network_controller->flush();
    }
}


void MainWindow::on_button_move_forward_clicked()
{
    if (is_connected && is_flying) {
        network_controller->add_command("forward 100");
        network_controller->flush();
    }
}


void MainWindow::on_button_rotate_left_clicked()
{
    if (is_connected && is_flying) {
        network_controller->add_command("ccw 20");
        network_controller->flush();
        drone_rotation -= 20;
        ui->text_drone_rotation->setText(QString::number(drone_rotation) + "°");
    }
}


void MainWindow::on_button_move_right_clicked()
{
    if (is_connected && is_flying) {
        network_controller->add_command("right 20");
        network_controller->flush();
    }
}


void MainWindow::on_button_rotate_right_clicked()
{
    if (is_connected && is_flying) {
        network_controller->add_command("cw 20");
        network_controller->flush();
        drone_rotation += 20;
        ui->text_drone_rotation->setText(QString::number(drone_rotation) + "°");
    }
}


void MainWindow::on_button_move_back_clicked()
{
    if (is_connected && is_flying) {
        network_controller->add_command("back 20");
        network_controller->flush();
    }
}


void MainWindow::on_button_move_down_clicked()
{
    if (is_connected && is_flying) {
        network_controller->add_command("down 20");
        network_controller->flush();
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
            disconnect(video_reader, SIGNAL(decoded_frame(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
            disconnect(edge_detector, SIGNAL(edgeframe(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
            connect(video_reader, SIGNAL(decoded_frame(cv::Mat&)), face_detector, SLOT(detect(cv::Mat&)));
            connect(face_detector, SIGNAL(faceframe(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
            face_detector->enabled = true;
            edge_detector->enabled = false;
        } else if (ui->edge_detection_radio->isChecked()) {
            disconnect(video_reader, SIGNAL(decoded_frame(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
            disconnect(face_detector, SIGNAL(faceframe(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
            connect(video_reader, SIGNAL(decoded_frame(cv::Mat&)), edge_detector, SLOT(detect(cv::Mat&)));
            connect(edge_detector, SIGNAL(edgeframe(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
            face_detector->enabled = false;
            edge_detector->enabled = true;
        }
    } else {
        edge_detector->enabled = false;
        face_detector->enabled = false;
        connect(video_reader, SIGNAL(decoded_frame(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
    }
}

void MainWindow::on_edge_detection_radio_clicked()
{
    disconnect(video_reader, SIGNAL(decoded_frame(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
    disconnect(face_detector, SIGNAL(faceframe(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
    connect(video_reader, SIGNAL(decoded_frame(cv::Mat&)), edge_detector, SLOT(detect(cv::Mat&)));
    connect(edge_detector, SIGNAL(edgeframe(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
    face_detector->enabled = false;
    edge_detector->enabled = true;
}

void MainWindow::on_face_detection_radio_clicked()
{
    disconnect(video_reader, SIGNAL(decoded_frame(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
    disconnect(edge_detector, SIGNAL(edgeframe(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
    connect(video_reader, SIGNAL(decoded_frame(cv::Mat&)), face_detector, SLOT(detect(cv::Mat&)));
    connect(face_detector, SIGNAL(faceframe(cv::Mat&)), this, SLOT(on_videoframe(cv::Mat&)));
    face_detector->enabled = true;
    edge_detector->enabled = false;
}

void MainWindow::on_recordtimer()
{
    ui->recording_dot->setVisible(!ui->recording_dot->isVisible());
}

void MainWindow::on_alerttimer()
{
    ui->alert_message->setVisible(!ui->alert_message->isVisible());
}

void MainWindow::on_faceoffset(QVector3D &offset)
{
    if (!follow_target) return;
    flight_controller->telemetry.set_metric_value("target", offset);
    flight_controller->update();
}

void MainWindow::on_flightstate(const FlightState &state)
{
    on_barometer(state.barometer);
    on_acceleration(state.acceleration);
    on_battery(state.battery);
    on_imu(state.imu);
    on_height(state.height);
    on_temperature(state.temperature.x());
    on_flighttime(state.flight_duration);
    on_timeofflight(state.tof);
    on_speed(state.speed.length());

    artificial_horizon->set_yaw_angle(state.imu.x());
    artificial_horizon->set_pitch_angle(-state.imu.y());
    artificial_horizon->set_roll_angle(-state.imu.z());
    artificial_horizon->repaint();

    altimeter->setAltitude((double)state.height / 100.0);
    altimeter->repaint();

    speedmeter->setSpeed((double)state.speed.length() / 100.0);
    speedmeter->repaint();

    temperature_gauge->setValue((double)state.temperature.x());
    temperature_gauge->repaint();

    battery_gauge->setValue((double)state.battery);
    battery_gauge->repaint();

    pressure_gauge->setValue((double)state.barometer);
    pressure_gauge->repaint();

    ui->video->repaint();
}

void MainWindow::on_button_start_recording_clicked()
{
    if (video_recorder) {
        if (video_recorder->is_recording()) {
             video_recorder->stop_record();
             ui->button_start_recording->setText("Start recording");
             ui->recording_text->setVisible(false);
             ui->recording_dot->setVisible(false);
             record_timer->stop();
        } else {
             video_recorder->start_record();
             ui->button_start_recording->setText("Stop recording");
             ui->recording_text->setVisible(true);

             auto ptr = new QTimer(this);
             connect(ptr, SIGNAL(timeout()), this, SLOT(on_recordtimer()));
             record_timer.reset(ptr);
             record_timer->start(1000);
        }
    }
}


void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    ui->video->setGeometry(QRect{0, 0,
        ui->tabWidget->geometry().width(),
        ui->tabWidget->geometry().height()
    });
}


void MainWindow::on_button_add_waypoint_clicked()
{
    auto item = new QListWidgetItem("Point_" + QString::number(waypoints.size() + 1));
    waypoints.push_back(item);
    ui->waypoints_list->addItem(item);
}


void MainWindow::on_button_remove_waypoint_clicked()
{
    for (auto item : ui->waypoints_list->selectedItems()) {
        for (size_t i = 0; i < waypoints.size(); ++i) {
            if (waypoints.at(i) == item) {
                delete waypoints.at(i);
                waypoints.remove(i);
            }
        }
    }

    qDeleteAll(ui->waypoints_list->selectedItems());

    if (ui->waypoints_list->count() > 0)
        ui->waypoints_list->item(0)->setSelected(true);
}

void MainWindow::on_waypoints_list_itemSelectionChanged()
{
    if (ui->waypoints_list->selectedItems().size() > 0)
        ui->button_remove_waypoint->setDisabled(false);
    else {
        ui->button_remove_waypoint->setDisabled(true);
    }
}


void MainWindow::on_checkbox_follow_target_clicked(bool checked)
{
    follow_target = checked;
}


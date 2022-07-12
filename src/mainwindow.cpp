#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
    , progress_bar(nullptr),
      controller(nullptr),
      is_connected(false),
      is_video_started(false),
      is_flying(false)
{
    ui->setupUi(this);
    ui->statusbar->showMessage("Disconnected");
    ui->button_takeoff->setDisabled(true);
    ui->button_emergency->setDisabled(true);
    ui->button_start_video->setDisabled(true);
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

        auto ptr = new UDPController(this, QHostAddress(address), port.toUShort());
        connect(ptr, SIGNAL(on_controller_ready()), this, SLOT(on_connected()));
        connect(ptr, SIGNAL(on_controller_max_speed(float)), this, SLOT(on_max_speed(float)));
        connect(ptr, SIGNAL(on_controller_battery(int)), this, SLOT(on_battery(int)));
        connect(ptr, SIGNAL(on_controller_flight_time(int)), this, SLOT(on_flight_time(int)));
        connect(ptr, SIGNAL(on_controller_height(int)), this, SLOT(on_height(int)));
        connect(ptr, SIGNAL(on_controller_temperature(int)), this, SLOT(on_temperature(int)));
        connect(ptr, SIGNAL(on_controller_imu(const QVector3D&)), this, SLOT(on_imu(const QVector3D&)));
        connect(ptr, SIGNAL(on_controller_barometer(float)), this, SLOT(on_barometer(float)));
        connect(ptr, SIGNAL(on_controller_acceleration(const QVector3D&)), this, SLOT(on_acceleration(const QVector3D&)));
        connect(ptr, SIGNAL(on_controller_time_of_flight(float)), this, SLOT(on_time_of_flight(float)));
        connect(ptr, SIGNAL(on_controller_wifi_snr(int)), this, SLOT(on_wifi_snr(int)));
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
    on_poll_infos();

    auto ptr = new QTimer(this);
    connect(ptr, SIGNAL(timeout()), this, SLOT(on_poll_infos()));
    timer.reset(ptr);
    timer->start(3000);
}

void MainWindow::on_battery(int percent) {
    progress_bar->setValue(percent);
    progress_bar->setFormat("Battery (" + QString::number(percent) + " %)");
}

void MainWindow::on_max_speed(float value) {
    ui->drone_speed->setText(QString::number(value) + " cm /s");
}

void MainWindow::on_flight_time(int seconds) {
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

void MainWindow::on_time_of_flight(float meters) {
    ui->drone_tof->setText(QString::number(meters) + " m");
}

void MainWindow::on_wifi_snr(int ratio) {
    ui->drone_wifi->setText(QString::number(ratio));
}

void MainWindow::on_poll_infos()
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

void MainWindow::on_button_takeoff_clicked()
{
    if (is_connected) {
        if (is_flying) {
            is_flying = false;
            controller->add_command("land");
            controller->flush();
            ui->button_takeoff->setText("Take off");
        } else {
            is_flying = true;
            controller->add_command("takeoff");
            controller->flush();
            ui->button_takeoff->setText("Land");
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
        } else {
            is_video_started = true;
            controller->add_command("streamon");
            controller->flush();
            ui->button_start_video->setText("Stop video");
        }
    }
}


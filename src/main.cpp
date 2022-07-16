#include <QApplication>
#include <QStatusBar>

#include "../include/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Tello Drone Controller");
    w.show();
    return a.exec();
}

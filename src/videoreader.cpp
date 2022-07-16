#include "../include/videoreader.h"
#include "ui_mainwindow.h"

VideoReader::VideoReader(QObject *parent)
    : QObject{parent}
{

}

void VideoReader::process()
{
    qDebug("Video reader started");

    cv::VideoCapture capture("udp://@0.0.0.0:11111");
    capture.set(cv::CAP_PROP_BUFFERSIZE, 1);

    while (true) {
        capture >> decoded;

        if(decoded.empty())
            continue;

        emit decoded_frame(decoded);
    }
}

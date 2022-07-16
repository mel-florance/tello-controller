#include "../include/videoreader.h"
#include "ui_mainwindow.h"

VideoReader::VideoReader(QObject *parent)
    : QObject{parent},
      running(true)
{
}

void VideoReader::stop()
{
    running = false;
}

void VideoReader::process()
{
    qDebug("Video reader started");

    _putenv_s("OPENCV_FFMPEG_CAPTURE_OPTIONS", "probesize;32|analyzeduration;0|fflags;nobuffer|flags;low_delay|strict;experimental");
    cv::VideoCapture capture("udp://@0.0.0.0:11111", cv::CAP_FFMPEG);
    capture.set(cv::CAP_PROP_BUFFERSIZE, 1);

    while (running) {
        capture >> decoded;

        if(decoded.empty()) {
            continue;
        }

        emit decoded_frame(decoded);
    }
}

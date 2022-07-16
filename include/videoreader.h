#ifndef VIDEOREADER_H
#define VIDEOREADER_H

#include <vector>
#include <string>
#include <QObject>
#include <QThread>

#include "../include/edgedetector.h"
#include "../include/facedetector.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>

using namespace std::chrono;

class VideoReader : public QObject
{
    Q_OBJECT
public:
    explicit VideoReader(QObject *parent = nullptr);
    void stop();

signals:
    void decoded_frame(cv::Mat matrix);

public slots:
    void process();

private:
    bool running;
    cv::Mat decoded;
};

#endif // VIDEOREADER_H

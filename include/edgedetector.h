#ifndef EDGEDETECTOR_H
#define EDGEDETECTOR_H

#include <QObject>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>

class EdgeDetector : public QObject
{
    Q_OBJECT
public:
    explicit EdgeDetector(QObject *parent = nullptr);

    bool enabled;
    int ratio;
    int min_threshold;
    int kernel_size;

signals:
    void edgeframe(cv::Mat frame);

public slots:
    void detect(cv::Mat src);
};

#endif // EDGEDETECTOR_H

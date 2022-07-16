#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

class VideoRecorder : public QObject
{
    Q_OBJECT
public:
    explicit VideoRecorder(QObject *parent = nullptr);

    void start_record();
    void write(cv::Mat& frame);
    void stop_record();
    inline bool is_recording() { return recording; };

signals:
private:
    bool recording;
    cv::VideoWriter writer;
};

#endif // VIDEORECORDER_H

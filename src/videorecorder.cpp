#include "../include/videorecorder.h"
#include "qapplication.h"
#include "qguiapplication.h"
#include "qcoreapplication.h"

VideoRecorder::VideoRecorder(QObject *parent)
    : QObject{parent},
      recording(false)
{
}

void VideoRecorder::start_record()
{
    int codec = cv::VideoWriter::fourcc('M', 'P', '4', 'V');
    auto video_path = qApp->applicationDirPath() + "/data/live.mp4";
    writer.open(video_path.toStdString().c_str(), codec, 30.0, cv::Size(960, 720), true);
    recording = true;
}

void VideoRecorder::write(cv::Mat &frame)
{
    writer.write(frame);
}

void VideoRecorder::stop_record()
{
    recording = false;
    writer.release();
}

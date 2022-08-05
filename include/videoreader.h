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

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
#include <libavutil/avutil.h>
}

struct VideoReaderState {
    int width;
    int height;
    AVRational time_base;
    AVFormatContext* av_format_ctx;
    AVCodecContext* av_codec_ctx;
    int video_stream_index;
    AVFrame* av_frame;
    AVPacket* av_packet;
    SwsContext* sws_scaler_ctx;
};

using namespace std::chrono;

class VideoReader : public QObject
{
    Q_OBJECT
public:
    explicit VideoReader(QObject *parent = nullptr);
    void stop();

    bool open(VideoReaderState* state, const char* filename);
    bool read_frame(VideoReaderState* state, uint8_t* frame_buffer, int64_t* pts);
    bool seek_frame(VideoReaderState* state, int64_t ts);
    void close(VideoReaderState* state);

signals:
    void decoded_frame(cv::Mat& matrix);

public slots:
    void process();

private:
    bool running;
    cv::Mat decoded;
};

#endif // VIDEOREADER_H

#include "../include/edgedetector.h"

EdgeDetector::EdgeDetector(QObject *parent)
    : QObject{parent},
    enabled(true),
    ratio(3),
    min_threshold(20),
    kernel_size(3)
{
}

void EdgeDetector::detect(cv::Mat src)
{
    if (src.empty() || !enabled)
        return;

    cv::Mat dst;
    cv::Mat src_gray;
    cv::Mat edges;

    dst.create(src.size(), src.type());
    cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
    cv::blur(src_gray, edges, cv::Size(3, 3));

    cv::Canny(
        edges,
        edges,
        min_threshold,
        min_threshold * ratio,
        kernel_size
    );

    dst = cv::Scalar::all(0);
    src.copyTo(dst, edges);

    emit edgeframe(dst);
}

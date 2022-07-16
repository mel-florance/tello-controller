#include "../include/facedetector.h"
#include "qapplication.h"
#include "qguiapplication.h"
#include "qcoreapplication.h"

FaceDetector::FaceDetector(QObject *parent)
    : QObject{parent},
      enabled(false),
      scale_factor(1.3f),
      min_neighbors(3),
      classifier(cv::CascadeClassifier())
{
    auto cascades_path = qApp->applicationDirPath() + "/data/cascades.xml";
    classifier.load(cascades_path.toStdString().c_str());
}

void FaceDetector::detect(cv::Mat& src)
{
    if (!enabled)
        return;

    std::vector<cv::Rect> faces = {};
    classifier.detectMultiScale(src, faces, scale_factor, min_neighbors);

    for (size_t i = 0; i < faces.size(); i++) {
        cv::rectangle(src, faces[i].tl(), faces[i].br(), cv::Scalar(50, 50, 255), 3);
    }

    emit face_frame(src);
}

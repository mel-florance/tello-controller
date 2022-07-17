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

void FaceDetector::detect(cv::Mat src)
{
    if (src.empty() || !enabled)
        return;

    std::vector<cv::Rect> faces = {};
    classifier.detectMultiScale(src, faces, scale_factor, min_neighbors);


    int center_x = int(960/2);
    int center_y = int(720/2);
    cv::circle(src, cv::Size(center_x, center_y), 10, cv::Scalar(0, 255, 0));
    int face_center_x = center_x;
    int face_center_y = center_y;
    int z_area = 0;

    for (size_t i = 0; i < faces.size(); i++) {
        cv::rectangle(src, faces[i].tl(), faces[i].br(), cv::Scalar(50, 50, 255), 3);

        face_center_x = faces[i].x + int(faces[i].width/2);
        face_center_y = faces[i].y + int(faces[i].height/2);
        z_area = faces[i].width * faces[i].height;

        cv::circle(src, cv::Size(face_center_x, face_center_y), 10, cv::Scalar(0, 255, 0));
    }

    cv::line(src, cv::Point(center_x, center_y), cv::Point(face_center_x, face_center_y), cv::Scalar(0, 255, 0), 1);

    int offset_x = face_center_x - center_x;
    int offset_y = face_center_y - center_y - 30;

    auto vec = QVector3D(offset_x, offset_y, z_area);
    emit faceoffset(vec);
    emit faceframe(src);
}

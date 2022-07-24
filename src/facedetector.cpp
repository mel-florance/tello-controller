#include "../include/facedetector.h"
#include "qapplication.h"
#include "qguiapplication.h"
#include "qcoreapplication.h"

FaceDetector::FaceDetector(QObject *parent)
    : QObject{parent},
      enabled(false),
      scale_factor(1.05f),
      min_neighbors(5),
      classifier(cv::CascadeClassifier())
{
    auto cascades_path = qApp->applicationDirPath() + "/data/haarcascade_frontalcatface.xml";
    classifier.load(cascades_path.toStdString().c_str());
}

void FaceDetector::detect(cv::Mat& src)
{
    if (src.empty() || !enabled)
        return;

    std::vector<cv::Rect> faces = {};
    classifier.detectMultiScale(src, faces, scale_factor, min_neighbors, cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));


    int center_x = int(960 * 0.5f);
    int center_y = int(720 * 0.5f);
    cv::circle(src, cv::Size(center_x, center_y), 10, cv::Scalar(0, 255, 0));
    int face_center_x = center_x;
    int face_center_y = center_y;
    int z_area = 0;

    for (size_t i = 0; i < faces.size(); i++) {
        cv::rectangle(src, faces[i].tl(), faces[i].br(), cv::Scalar(50, 50, 255), 1);

        face_center_x = faces[i].x + int(faces[i].width * 0.5f);
        face_center_y = faces[i].y + int(faces[i].height * 0.5f);
        z_area = faces[i].width * faces[i].height;

        cv::circle(src, cv::Size(face_center_x, face_center_y), 10, cv::Scalar(0, 255, 0));
    }

    cv::line(src, cv::Point(center_x, center_y), cv::Point(face_center_x, face_center_y), cv::Scalar(0, 255, 0), 1);

    int offset_x = face_center_x - center_x;
    int offset_y = face_center_y - center_y - 30;

    auto vec = QVector3D(offset_x, offset_y, z_area);

    if (faces.size() > 0)
        emit faceoffset(vec);

    emit faceframe(src);
}

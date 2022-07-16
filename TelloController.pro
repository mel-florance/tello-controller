QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ./src/main.cpp \
    ./src/mainwindow.cpp \
    ./src/tellocontroller.cpp \
    ./src/videoreader.cpp \
    ./src/videorecorder.cpp \
    ./src/edgedetector.cpp \
    ./src/facedetector.cpp

HEADERS += \
    ./include/mainwindow.h \
    ./include/tellocontroller.h \
    ./include/videoreader.h \
    ./include/videorecorder.h \
    ./include/edgedetector.h \
    ./include/facedetector.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

Release:DESTDIR = $$PWD/release

DEPENDPATH += $$PWD/3rdparty/opencv/include
INCLUDEPATH += $$PWD/3rdparty/opencv/include
LIBS += -L$$PWD/3rdparty/opencv/lib \
    -lopencv_calib3d460 \
    -lopencv_core460\
    -lopencv_dnn460 \
    -lopencv_features2d460 \
    -lopencv_flann460 \
    -lopencv_gapi460 \
    -lopencv_highgui460 \
    -lopencv_imgcodecs460 \
    -lopencv_imgproc460 \
    -lopencv_ml460 \
    -lopencv_objdetect460 \
    -lopencv_photo460 \
    -lopencv_stitching460 \
    -lopencv_video460 \
    -lopencv_videoio460 \

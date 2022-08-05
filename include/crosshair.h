#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include <QWidget>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>

class Crosshair : public QWidget
{
    Q_OBJECT
public:
    explicit Crosshair(QWidget *parent = nullptr);

    inline int getWidth() const { return width; }
    inline void setWidth(int newWidth) { width = newWidth; }
    inline int getHeight() const { return height; }
    inline void setHeight(int newHeight) { height = newHeight; }

protected:
    void paintEvent(QPaintEvent* event) override;

signals:
private:
    QColor color;
    int width;
    int height;
    int size;
    int gap;
};

#endif // CROSSHAIR_H

#ifndef WAYPOINTEDITOR_H
#define WAYPOINTEDITOR_H

#include <QObject>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QLabel>
#include <QPushButton>

class WaypointEditor : public QWidget
{
    Q_OBJECT
public:
    explicit WaypointEditor(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

signals:
private:
    QLabel label;
    QPushButton switch_mode;
};

#endif // WAYPOINTEDITOR_H

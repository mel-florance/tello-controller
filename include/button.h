#ifndef BUTTON_H
#define BUTTON_H

#include <memory>

#include <QWidget>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

class Button : public QWidget
{
    Q_OBJECT
public:
    explicit Button(QWidget *parent = nullptr, const QString& text = QString());
    inline const QString& getText() const { return text; }
    inline void setText(const QString &newText) { text = newText; }
    inline double getWidth() const { return width; }
    inline void setWidth(double newWidth) { width = newWidth; };
    inline double getHeight() const { return height; }
    inline void setHeight(double newHeight) { height = newHeight; }
    inline bool getAlerting() const { return alerting; }
    inline int getAlert_interval() const { return alert_interval; }
    inline void setAlert_interval(int newAlert_interval) { alert_interval = newAlert_interval; }
    void startAlerting();
    void stopAlerting();

protected:
    void paintEvent(QPaintEvent* event) override;

signals:

public slots:
    void on_alerttimer();

private:
    QString text;
    double width;
    double height;
    QColor border_color;
    QColor background_color;
    QColor text_color;
    bool highlight;
    bool alerting;
    std::unique_ptr<QTimer> alert_timer;
    int alert_interval;
    QPen current_pen;
};

#endif // BUTTON_H

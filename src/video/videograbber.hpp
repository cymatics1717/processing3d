#ifndef VIDEOGRABBER_HPP
#define VIDEOGRABBER_HPP

#include <QObject>
#include <opencv2/opencv.hpp>
#include <QImage>

class videoGrabber : public QObject
{
    Q_OBJECT
public:
    explicit videoGrabber(QString source, QObject *parent = nullptr);
    ~videoGrabber();
signals:
    void incoming(QImage frame);
public slots:
    void openURL();
    void readFrame();
protected:
    void timerEvent(QTimerEvent *event) override;

private:
    QString m_source;
    cv::VideoCapture cap;
    int timerid;
};

#endif // VIDEOGRABBER_HPP

#include "videograbber.hpp"
#include <QDebug>
videoGrabber::videoGrabber(QString source, QObject *parent)
    : QObject(parent)
    , m_source(source)
{
}

videoGrabber::~videoGrabber()
{
//    qDebug() << "";
}

static QImage matToQImage( const cv::Mat &mat )
{
    switch (mat.type())
    {
    case CV_8UC4:
    {
        return QImage( mat.data,mat.cols, mat.rows,static_cast<int>(mat.step), QImage::Format_ARGB32 );
    }
    case CV_8UC3:
    {
        return QImage( mat.data,mat.cols, mat.rows,static_cast<int>(mat.step),QImage::Format_RGB888).rgbSwapped();
    }
    case CV_8UC1:
    {
        return QImage( mat.data,mat.cols, mat.rows,static_cast<int>(mat.step),QImage::Format_Grayscale8 );
    }
    default:
        qWarning() << "cv::Mat image type not handled in switch:" << mat.type();
        break;
    }
    return QImage();
}

void videoGrabber::openURL()
{
//    qDebug() << "";
    if (!cap.isOpened()) {
//        https://stackoverflow.com/questions/38365051/opencv-videocapture-timeout-on-open-or-constructor/40667970#40667970
qDebug() << m_source<< cap.open(m_source.toStdString());
        timerid = startTimer(1000 / 24);
    }
}

void videoGrabber::readFrame()
{
    if (cap.isOpened()) {
        cv::Mat frame;
        cap.read(frame);
        incoming(matToQImage(frame));
    }
}

void videoGrabber::timerEvent(QTimerEvent *event)
{
//    qDebug() << "";
    readFrame();
}

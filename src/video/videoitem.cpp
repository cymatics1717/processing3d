#include "videoitem.hpp"
#include <QDebug>
#include <QThread>

videoItem::videoItem(QQuickPaintedItem *parent):QQuickPaintedItem(parent)
    ,job(nullptr),grabber(nullptr)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptTouchEvents(true);
    //    setAcceptHoverEvents(true);
    setFlags(flags()|QQuickItem::ItemAcceptsDrops
             |QQuickItem::ItemIsFocusScope
             |QQuickItem::ItemClipsChildrenToShape
             //             |QQuickItem::ItemAcceptsInputMethod
             |QQuickItem::ItemHasContents);
}

videoItem::~videoItem()
{
//    qDebug() << "";
    job->quit();
    job->wait(100);
//    qDebug() << "";
}

void videoItem::paint(QPainter *painter)
{
//    qDebug() << frame.width() << frame.height();
    if (!frame.isNull()) {
        painter->drawImage(0, 0, frame.scaled(width(),height(),Qt::KeepAspectRatio));
        frame = QImage();
    }
}

void videoItem::init()
{
    if (job != nullptr) {
        job->quit();
        job->wait();
        grabber->deleteLater();
    } else {
        job = new QThread;
    }
    grabber = new videoGrabber(m_source);
    grabber->moveToThread(job);

    connect(grabber, &videoGrabber::incoming,this,&videoItem::updateFrame);
    connect(job, &QThread::started, grabber, &videoGrabber::openURL);
    connect(job, &QThread::finished, grabber, &videoGrabber::deleteLater);
    connect(job, &QThread::finished, job, &QThread::deleteLater);
    job->start();
}

QString videoItem::source() const
{
    return m_source;
}

void videoItem::setSource(QString nsource)
{
    m_source = nsource;
    init();
    emit sourceChanged(m_source);
}

void videoItem::updateFrame(QImage frame)
{
    this->frame = frame;
    update();
}

bool videoItem::eventFilter(QObject *o, QEvent *e)
{
    qDebug() << o << e;
}

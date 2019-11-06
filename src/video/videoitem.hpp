#ifndef VIDEOITEM_HPP
#define VIDEOITEM_HPP

#include <QPainter>
#include <QQuickPaintedItem>

#include <QThread>
#include "videograbber.hpp"

class videoItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource MEMBER m_source NOTIFY sourceChanged)
public:
    explicit videoItem(QQuickPaintedItem *parent = nullptr);
    ~videoItem();
    void paint(QPainter *painter) override;

public slots:
    void init();
    QString source() const;
    void setSource(QString nsource);
    void updateFrame(QImage frame);
protected:
    bool eventFilter(QObject *o, QEvent *e) override;

signals:
    void sourceChanged(QString source);
//    void incoming(QString source);

private:
    QImage frame;
    QString m_source;
    QThread *job;
    videoGrabber *grabber;
};

#endif // VIDEOITEM_HPP

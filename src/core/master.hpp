#ifndef MASTER_HPP
#define MASTER_HPP

#include <QHash>
#include <QJsonObject>
#include <QObject>
#include <QThread>
#include <QTime>
#include "worker.hpp"

class Master : public QObject
{
    Q_OBJECT
public:
    explicit Master(QObject *parent = nullptr);
    ~Master();
signals:

public slots:
    void start(QJsonObject json);
    void testFile(QString filename);

    int convertFormat(const QString &srcfile, const QString &dstfile);

private:
    QHash<QDateTime,Worker*> pool;
};

#endif // MASTER_HPP

#include "master.hpp"

#include <QFile>
#include <QDebug>

Master::Master(QObject *parent) : QObject(parent)
{

}

Master::~Master()
{
    qDeleteAll(pool);
}

void Master::start(QJsonObject json)
{
    QThread *job = new QThread;
    Worker *worker = new Worker(json);
    pool.insert(QDateTime::currentDateTime(),worker);
    worker->moveToThread(job);
    connect(job, &QThread::started, worker, &Worker::process);
    connect(job, &QThread::finished, job, &QThread::deleteLater);
    connect(job, &QThread::finished, worker, &Worker::deleteLater);
    job->start();
}

void Master::testFile(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QString data = file.readAll();
        QJsonDocument doc;

        start(doc.fromJson(data.toUtf8()).object());

    } else {
        qDebug() << "error read file" << filename;
    }
    file.close();
}

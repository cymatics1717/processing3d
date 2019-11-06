#include "master.hpp"

#include <QFile>
#include <QDebug>
#include <QJsonArray>

static QString json2string(QJsonObject obj){
    return QJsonDocument(obj).toJson();
}

static QJsonObject string2json(QString str){
    return QJsonDocument::fromJson(str.toUtf8()).object();
}

static QString fileReadAll(QString filename)
{
    QString ans = "";
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ans = file.readAll();
    } else {
        qDebug() << "error read file" << filename;
    }
    file.close();
    return ans;
}

Master::Master(QObject *parent) : QObject(parent)
{

}

Master::~Master()
{
    qDeleteAll(pool);
}

void Master::start(QJsonObject json)
{
    qDebug() << json;

    QThread *job = new QThread;
    Worker *worker = new Worker(json2string(json));
    pool.insert(QDateTime::currentDateTime(),worker);
    worker->moveToThread(job);
    connect(job, &QThread::started, worker, &Worker::process);
    connect(job, &QThread::finished, job, &QThread::deleteLater);
    connect(job, &QThread::finished, worker, &Worker::deleteLater);
    job->start();
    qDebug() << "done.";

}

void Master::testFile(QString filename)
{
    //    start(string2json(fileReadAll(filename)));

    convertFormat("/home/wayne/3d/Cloud2.e57", "/home/wayne/3d/Cloud2.pcd");
}

int Master::convertFormat(const QString &srcfile, const QString &dstfile)
{
    qDebug() << "";
    QJsonArray pipeline;
    pipeline.append(srcfile);
    pipeline.append(dstfile);
    QJsonObject obj;
    obj["pipeline"] = pipeline;
    start(obj);

    return 0;
}

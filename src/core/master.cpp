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
    qDebug() << "";
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
//    connect(job, &QThread::finished, worker, &Worker::deleteLater);
    job->start();
    qDebug() << "done.";

}

void Master::testFile(QString filename)
{
    //    start(string2json(fileReadAll(filename)));

//    convertFormat("/home/wayne/3d/Cloud2.ply",QString("/home/wayne/3d/Cloud2-%1.ply").arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")));
//    E57ToPLY("/home/wayne/3d/cloud/pointcloud-merged.e57",QString("/home/wayne/3d/pointcloud-%1.ply").arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")));
    PLYToPCD("/home/wayne/3d/Cloud2.ply",QString("/home/wayne/3d/Cloud21.pcd"));



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

int Master::E57ToPLY(const QString &srcfile, const QString &dstfile)
{
    qDebug() << "";
    QJsonArray pipeline;
    QJsonObject src;
    src["type"] = "readers.e57";
    src["filename"] = srcfile;
    QJsonObject dst;
    dst["type"] = "writers.ply";
    dst["storage_mode"] = "little endian";
    dst["filename"] = dstfile;
    pipeline.append(src);
    pipeline.append(dst);

    QJsonObject obj;
    obj["pipeline"] = pipeline;
    start(obj);

    return 0;
}

int Master::PLYToPCD(const QString &srcfile, const QString &dstfile)
{
    qDebug() << "";
    QJsonArray pipeline;
    QJsonObject src;
    src["type"] = "readers.ply";
    src["filename"] = srcfile;
    QJsonObject dst;
    dst["type"] = "writers.pcd";
    dst["compression"] = "binary";
    dst["filename"] = dstfile;
    pipeline.append(src);
    pipeline.append(dst);

    QJsonObject obj;
    obj["pipeline"] = pipeline;
    start(obj);

    return 0;
}



#include "worker.hpp"
#include <QDebug>
#include <sstream>

Worker::Worker(QJsonObject obj, QObject *parent) : QObject(parent),job(obj)
{

}

Worker::~Worker()
{
    qDebug() <<"";
}

void Worker::process()
{
    QJsonDocument doc(job);
    std::stringstream ss(doc.toJson().toStdString());
    manager.readPipeline(ss);

    qDebug() << manager.execute();

    pdal::FileUtils::deleteFile("");
}


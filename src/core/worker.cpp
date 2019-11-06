#include "worker.hpp"
#include <QDebug>
#include <sstream>

Worker::Worker(QString obj, QObject *parent) : QObject(parent),pipeline(obj)
{

}

Worker::~Worker()
{
    qDebug() <<"";
}

void Worker::process()
{
    std::stringstream ss(pipeline.toStdString());
    manager.readPipeline(ss);

    qDebug() << manager.execute();

}


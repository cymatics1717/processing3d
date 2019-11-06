#ifndef WORKER_HPP
#define WORKER_HPP

#include <QJsonObject>
#include <QJsonDocument>

#include <QObject>
#include <pdal/PipelineManager.hpp>
#include <pdal/util/FileUtils.hpp>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QString obj, QObject *parent = nullptr);
    ~Worker();

signals:

public slots:
    void process();
private:
    QString pipeline;
    pdal::PipelineManager manager;
};

#endif // WORKER_HPP

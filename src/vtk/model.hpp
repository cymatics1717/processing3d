#ifndef MODEL_HPP
#define MODEL_HPP

#include <QObject>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

class modelManager;
class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QString fn,modelManager *manager, QObject *parent = nullptr);
    ~Model();

    QString source()const;

signals:
      void done(QString file, int result);
public slots:
    void load();
    int loadPano();

private:
    int loadPrimitive();
    int loadImage();
    int loadMesh();
    int loadCloud();
    int loadCloudPdal();
private:
    QString filename;
    vtkSmartPointer<vtkProp3D> prop;
    modelManager *manager;

};

#endif // MODEL_HPP

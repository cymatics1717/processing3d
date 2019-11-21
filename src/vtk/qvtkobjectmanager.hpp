#ifndef QVTKOBJECTMANAGER_HPP
#define QVTKOBJECTMANAGER_HPP

#include <QHash>
#include <QObject>
#include <vtkRenderer.h>
#include "qvtkobject3d.hpp"

class QVTKObjectManager : public QObject
{
    Q_OBJECT
public:
    explicit QVTKObjectManager(vtkSmartPointer<vtkRenderer> mrenderer, QObject *parent = nullptr);
    ~QVTKObjectManager();
signals:
    void done(int status,QString name);

public slots:
    QVTKObject3D* getObject(vtkProp3D *prop) const;

    void applyTransform(vtkProp3D *prop, vtkSmartPointer<vtkTransform> t);

private:
    QHash<vtkProp3D*,QVTKObject3D*> pool;
};

#endif // QVTKOBJECTMANAGER_HPP

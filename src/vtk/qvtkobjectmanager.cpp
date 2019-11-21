#include "qvtkobjectmanager.hpp"
#include <vtkRenderWindow.h>
#include <QFileInfo>
#include <QThread>
#include <QDebug>
#include <QUrl>
QVTKObjectManager::QVTKObjectManager(vtkSmartPointer<vtkRenderer> mrenderer, QObject *parent) :
    QObject(parent)
{

}

QVTKObjectManager::~QVTKObjectManager()
{
    qDeleteAll(pool);
}

QVTKObject3D *QVTKObjectManager::getObject(vtkProp3D *prop) const
{
    return pool.value(prop,nullptr);
}

void QVTKObjectManager::applyTransform(vtkProp3D *prop, vtkSmartPointer<vtkTransform> t)
{
    auto item = pool.value(prop,nullptr);
    if(item){
        item->applyTransform(t);
    }
}

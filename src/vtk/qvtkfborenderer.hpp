#ifndef QVTKFBORENDERER_HPP
#define QVTKFBORENDERER_HPP

#include <QObject>
#include <QOpenGLFunctions>
#include <QQuickFramebufferObject>
#include <vtkCellPicker.h>
#include <vtkCubeSource.h>
#include <vtkExternalOpenGLRenderWindow.h>
#include <vtkGenericRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <QVTKInteractorAdapter.h>
#include <QVTKInteractor.h>
#include <vtkEventQtSlotConnect.h>
#include <memory>
#include <vtkPropPicker.h>
//#include "modelmanager.hpp"
#include "qvtkobjectmanager.hpp"
#include "qvtkfboitem.hpp"

class QVTKFBORenderer : public QObject, public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit QVTKFBORenderer(QObject *parent = nullptr);
    ~QVTKFBORenderer() override;
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    void render() override;
    void synchronize(QQuickFramebufferObject *item) override;

    void resetCamera();

signals:

public slots:
    void initScene();
    void selectedModel(QString modelFileName);
    void setCamera(QVTKFBOItem::Orientation orit);

    void addObject(QString filename);
    QVTKObject3D* getObject(vtkProp3D *prop) const;
    void onObjectAdded(int status, QString name);

private:
    void handleKey(std::shared_ptr<QEvent> keyevent);
    void handleMouse(std::shared_ptr<QEvent> mouseevent, vtkSmartPointer<vtkTransform> trans);
    QVTKFBOItem *m_fboItem;

    vtkSmartPointer<vtkExternalOpenGLRenderWindow> renderWindow;
    QVTKInteractorAdapter* m_dapter;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkProp3D* selected_Prop;

//    std::shared_ptr<QEvent> mouseevent;
//    std::shared_ptr<QEvent> keyevent;

    double mfactor;


    vtkSmartPointer<vtkPropPicker> picker;
    int cnt;
    QHash<vtkProp3D *, QVTKObject3D *> pool;

};

#endif // QVTKFBORENDERER_HPP

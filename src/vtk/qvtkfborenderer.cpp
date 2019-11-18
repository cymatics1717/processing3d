#include "qvtkfborenderer.hpp"
#include "cellpickinteractorstyle.hpp"
#include "proppickinteractorstyle.hpp"

#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <memory>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCamera.h>
#include <QMetaObject>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkPolyLine.h>
#include <vtkVersion.h>
#include <vtkPropPicker.h>
#include <vtkRendererCollection.h>
#include <vtkPointPicker.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkProp3DCollection.h>
#include <vtkQuaternion.h>
#include <QTimeLine>

class vtkHoverCallback : public vtkCommand {
public:
    static vtkHoverCallback *New() {
        return new vtkHoverCallback;
    }

    vtkHoverCallback() {}

    virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData) override;

};

void vtkHoverCallback::Execute(vtkObject *, unsigned long event, void *vtkNotUsed(calldata)) {
    switch (event) {
        case vtkCommand::TimerEvent:
            std::cout << "TimerEvent -> the mouse stopped moving and the widget hovered" << std::endl;
            break;
        case vtkCommand::EndInteractionEvent:
            std::cout << "EndInteractionEvent -> the mouse started to move" << std::endl;
            break;
    }
}

QVTKFBORenderer::QVTKFBORenderer(QObject *parent) : QObject(parent), manager(new modelManager(this)),
                                                    m_fboItem(nullptr), m_dapter(new QVTKInteractorAdapter(this)),
                                                    m_renderer(vtkSmartPointer<vtkRenderer>::New()),
                                                    selectedMouse(nullptr), mfactor(500) {
    qDebug() << vtkVersion::GetVTKSourceVersion();
    // Renderer
    vtkSmartPointer<QVTKInteractor> mInteractor = vtkSmartPointer<QVTKInteractor>::New();

    mInteractor->EnableRenderOff();
    renderWindow = vtkSmartPointer<vtkExternalOpenGLRenderWindow>::New();
    renderWindow->AddRenderer(m_renderer);
    renderWindow->OpenGLInitContext();
    renderWindow->SetOffScreenRendering(true);

//    renderWindow->SetReadyForRendering(false);
//    renderWindow->SetForceMaximumHardwareLineWidth(1);
    renderWindow->SetInteractor(mInteractor);


//        vtkSmartPointer<cellPickInteractorStyle> style = vtkSmartPointer<cellPickInteractorStyle>::New();
//    vtkSmartPointer<propPickInteractorStyle> style = vtkSmartPointer<propPickInteractorStyle>::New();
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    style->SetDefaultRenderer(m_renderer);
//    style->SetMotionFactor(10.0);
//    style->setParent(this);
    mInteractor->SetInteractorStyle(style);

    mInteractor->Initialize();
    // Initial camera position
    resetCamera();

    manager->setRenderer(m_renderer);

//    vtkSmartPointer<vtkHoverCallback> hoverCallback =
//        vtkSmartPointer<vtkHoverCallback>::New();
//      renderWindow->AddObserver(vtkCommand::TimerEvent,hoverCallback);
//      renderWindow->AddObserver(vtkCommand::EndInteractionEvent,hoverCallback);

}

QVTKFBORenderer::~QVTKFBORenderer() {

}

QOpenGLFramebufferObject *QVTKFBORenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Depth);
//    format.setSamples(0);
//    format.setMipmap(true);

    std::unique_ptr<QOpenGLFramebufferObject> framebufferObject(new QOpenGLFramebufferObject(size, format));

//    renderWindow->SetBackLeftBuffer(GL_COLOR_ATTACHMENT0);
//    renderWindow->SetFrontLeftBuffer(GL_COLOR_ATTACHMENT0);
//    renderWindow->SetBackBuffer(GL_COLOR_ATTACHMENT0);
//    renderWindow->SetFrontBuffer(GL_COLOR_ATTACHMENT0);
//    renderWindow->SetSize(framebufferObject->size().width(), framebufferObject->size().height());
//    renderWindow->SetOffScreenRendering(true);
//    renderWindow->Modified();

    return framebufferObject.release();
}

void QVTKFBORenderer::resetCamera() {
    renderWindow->OpenGLInitState();
    renderWindow->MakeCurrent();
    QOpenGLFunctions::initializeOpenGLFunctions();
    QOpenGLFunctions::glUseProgram(0);

    m_renderer->ResetCameraClippingRange();
    m_renderer->GetActiveCamera()->SetPosition(0, 0, mfactor);
    m_renderer->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);
    m_renderer->GetActiveCamera()->SetViewUp(0.0, mfactor, 0.0);
//    m_renderer->GetActiveCamera()->set
}

void QVTKFBORenderer::initScene() {
    QString msg = QString("OpenGL Version: ") + reinterpret_cast<const char *>(glGetString(GL_VERSION));
    qDebug() << msg;
    m_fboItem->message(msg);

    m_renderer->SetBackground(9 / 255., 13 / 255., 37 / 255.);
    m_renderer->SetBackground2(9 / 255., 13 / 255., 33 / 255.);
    m_renderer->SetBackgroundAlpha(0.5);
    m_renderer->GradientBackgroundOn();
    connect(m_fboItem, &QVTKFBOItem::openFile, manager, &modelManager::addModel);
    connect(m_fboItem, &QVTKFBOItem::cameraOrientationChanged, this, &QVTKFBORenderer::setCamera);

    manager->addModel("axes");
#define PATH "/home/wayne/Pictures/3d/"
//    static int cnt = 0;
//    cnt++;
//    if(cnt==1){
//        manager->addModel("/home/wayne/www/3d/a/LI_QING_1974_10_9");
//        manager->addModel(PATH"KnotData.stl");
//    } else {
//        manager->addModel(PATH"KnotData.stl");
//    }
}

void QVTKFBORenderer::selectedModel(QString modelFileName) {
    m_fboItem->selectedModel(modelFileName);
}

void QVTKFBORenderer::setCamera(QVTKFBOItem::Orientation orit) {
    qDebug() << (orit == QVTKFBOItem::Orientation::BottomView);

    double eye[] = {0, 0, 0};
    double center[] = {0, 0, 1};
    double top[] = {0, 0, 0};

    switch (orit) {
        case QVTKFBOItem::Orientation::TopView:
            eye[2] = 1.0 * mfactor;
            top[1] = 1.0 * mfactor;
            break;
        case QVTKFBOItem::Orientation::BottomView:
            eye[2] = -1.0 * mfactor;
            top[1] = 1.0 * mfactor;
            break;
        case QVTKFBOItem::Orientation::FrontView:
            eye[1] = -1.0 * mfactor;
            top[2] = 1.0 * mfactor;
            break;
        case QVTKFBOItem::Orientation::BackView:
            eye[1] = 1.0 * mfactor;
            top[2] = 1.0 * mfactor;
            break;
        case QVTKFBOItem::Orientation::LeftView:
            eye[0] = -1.0 * mfactor;
            top[2] = 1.0 * mfactor;
            break;
        case QVTKFBOItem::Orientation::RightView:
            eye[0] = 1.0 * mfactor;
            top[2] = 1.0 * mfactor;
            break;
        case QVTKFBOItem::Orientation::ISOFrontView:
            eye[0] = -1.0 * mfactor;
            eye[1] = -1.0 * mfactor;
            eye[2] = 1.0 * mfactor;
            top[0] = 1.0 * mfactor;
            top[1] = 1.0 * mfactor;
            top[2] = 1.0 * mfactor;
            break;
        case QVTKFBOItem::Orientation::ISOBackView:
            eye[0] = 1.0 * mfactor;
            eye[1] = 1.0 * mfactor;
            eye[2] = 1.0 * mfactor;
            top[0] = -1.0 * mfactor;
            top[1] = -1.0 * mfactor;
            top[2] = 1.0 * mfactor;
            break;
        case QVTKFBOItem::Orientation::OriginView:
            eye[0] = 0;
            eye[1] = 0;
            eye[2] = 1.0 * mfactor;
            top[0] = 0;
            top[1] = 1.0 * mfactor;
            top[2] = 0;
            break;
    }

    m_renderer->GetActiveCamera()->SetPosition(eye);
    m_renderer->GetActiveCamera()->SetFocalPoint(center);
    m_renderer->GetActiveCamera()->SetViewUp(top);
//    render();
}

void QVTKFBORenderer::updateCamera(double x, double y, double z) {
    double *cam = m_renderer->GetActiveCamera()->GetPosition();

    delta = QVector3D(x - cam[0], y - cam[1], z - cam[2]);
    counter = 0;
    total_counter = 1000;

//    update();
}

void QVTKFBORenderer::synchronize(QQuickFramebufferObject *item) {
//    static int cnt=0;
//    qDebug() <<++cnt<<QString(60,'-')<< __LINE__ << __FUNCTION__;
    if (!m_fboItem) {
        m_fboItem = static_cast<QVTKFBOItem *>(item);
        initScene();
    }

    auto trans = vtkSmartPointer<vtkTransform>::New();
    trans->Identity();

    trans->RotateWXYZ(m_fboItem->m_pose.scalar()
                      ,m_fboItem->m_pose.x()
                      ,m_fboItem->m_pose.y()
                      ,m_fboItem->m_pose.z());
    m_renderer->GetActiveCamera()->ApplyTransform(trans);


    if (renderWindow && renderWindow->GetReadyForRendering()) {
        while (!m_fboItem->events.empty()) {
            auto e = m_fboItem->events.takeFirst();
            e->accept();
            QMouseEvent *mouse = static_cast<QMouseEvent *>(e.get());
            if (mouse) {
                if(mouse->type() ==QEvent::MouseButtonDblClick)
                qDebug() <<mouse->type()<< mouse << (mouse->flags()==Qt::MouseEventCreatedDoubleClick);
            }

            m_dapter->ProcessEvent(e.get(), renderWindow->GetInteractor());
            if (e->type() == QEvent::MouseButtonPress) {
                selectedMouse = e;
            } else if (e->type() == QEvent::KeyPress) {
                keyevent = e;
            }
        }
    }

    int *rendererSize = renderWindow->GetSize();
    if (int(m_fboItem->width()) != rendererSize[0] || int(m_fboItem->height()) != rendererSize[1]) {
        renderWindow->SetSize(int(m_fboItem->width()), int(m_fboItem->height()));
    }
}

void QVTKFBORenderer::render() {
//    static int cnt = 0;
//    cnt++;
//    qDebug() <<cnt<<QString(60,'-')<< __LINE__ << __FUNCTION__;
    if (selectedMouse) {
//        qDebug() << selectedMouse.get();
        QMouseEvent *mouse = static_cast<QMouseEvent *>(selectedMouse.get());
        if (Qt::LeftButton & mouse->buttons()) {
        } else if (Qt::RightButton & mouse->buttons()) {
            auto menuevent = QSharedPointer<QContextMenuEvent>::create(
                    QContextMenuEvent::Mouse, mouse->pos(), mouse->globalPos());
            m_dapter->ProcessEvent(menuevent.get(), renderWindow->GetInteractor());
        }
        selectedMouse.reset();
    } else {
//        qDebug()<<cnt <<QString(30,'-');
    }

    auto camera = m_renderer->GetActiveCamera();
    auto pos = camera->GetPosition();
    if (keyevent) {
        auto key_event = static_cast<QKeyEvent *>(keyevent.get());
        auto focal = camera->GetFocalPoint();
        //        auto up = camera->GetViewUp();
        if (key_event->key() == Qt::Key_Left) {
            camera->SetPosition(pos[0] - 1, pos[1], pos[2]);
            camera->SetFocalPoint(focal[0] - 1, focal[1], focal[2]);
//            camera->SetViewUp(up[0]-1,up[1], up[2]);
        } else if (key_event->key() == Qt::Key_Right) {
            camera->SetPosition(pos[0] + 1, pos[1], pos[2]);
            camera->SetFocalPoint(focal[0] + 1, focal[1], focal[2]);
//            camera->SetViewUp(up[0]+1,up[1], up[2]);
        } else if (key_event->key() == Qt::Key_Up) {
            camera->SetPosition(pos[0], pos[1] + 1, pos[2]);
            camera->SetFocalPoint(focal[0], focal[1] + 1, focal[2]);
//            camera->SetViewUp(up[0],up[1]+1, up[2]);
        } else if (key_event->key() == Qt::Key_Down) {
            camera->SetPosition(pos[0], pos[1] - 1, pos[2]);
            camera->SetFocalPoint(focal[0], focal[1] - 1, focal[2]);
//            camera->SetViewUp(up[0],up[1]-1, up[2]);
        } else if (key_event->key() == Qt::Key_PageUp) {
            camera->SetPosition(pos[0], pos[1], pos[2] + 1);
            camera->SetFocalPoint(focal[0], focal[1], focal[2] + 1);
//            camera->SetViewUp(up[0],up[1], up[2]+1);
        } else if (key_event->key() == Qt::Key_PageDown) {
            camera->SetPosition(pos[0], pos[1], pos[2] - 1);
            camera->SetFocalPoint(focal[0], focal[1], focal[2] - 1);
//            camera->SetViewUp(up[0],up[1], up[2]-1);
        } else if (key_event->key() == Qt::Key_0) {
            camera->SetPosition(0, 0, 1.0 * mfactor);
            camera->SetFocalPoint(0, 0, 1);
            camera->SetViewUp(0, 1.0 * mfactor, 0);
        }
        keyevent.reset();
    }


//    if (counter < total_counter) {
//        m_renderer->GetActiveCamera()->SetPosition(pos[0]+delta.x() * counter / total_counter,
//                                                   pos[1]+delta.y() * counter / total_counter,
//                                                   pos[2]+delta.z() * counter / total_counter);
//        counter++;
//    }

    renderWindow->PushState();
//    renderWindow->Start();
//    m_renderer->GetActiveCamera()->SetPosition(-1000, 0, 1000);
//    m_renderer->GetActiveCamera()->SetViewUp(0.0, 1, 0.0);

    // Render
    renderWindow->Render();
    renderWindow->PopState();

    m_fboItem->window()->resetOpenGLState();
}


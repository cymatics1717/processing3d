#include "qvtkfborenderer.hpp"
#include "cellpickinteractorstyle.hpp"
#include "proppickinteractorstyle.hpp"
#include <vtkPointPicker.h>
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
#include <QThread>
#include <QFileInfo>

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

QVTKFBORenderer::QVTKFBORenderer(QObject *parent) : QObject(parent)/*, manager(new modelManager(this))*/,
                                                    m_fboItem(nullptr), m_dapter(new QVTKInteractorAdapter(this)),
                                                    m_renderer(vtkSmartPointer<vtkRenderer>::New()),selected_Prop(nullptr),
                                                    mfactor(500),picker(vtkSmartPointer<vtkPropPicker>::New()),cnt(0)
{

    qDebug() << vtkVersion::GetVTKSourceVersion();
    // Renderer
    auto m_Interactor = vtkSmartPointer<QVTKInteractor>::New();
    m_Interactor->Initialize();
    m_Interactor->EnableRenderOn();
    renderWindow = vtkSmartPointer<vtkExternalOpenGLRenderWindow>::New();
    renderWindow->AddRenderer(m_renderer);
    renderWindow->OpenGLInitContext();
//    renderWindow->SetOffScreenRendering(true);

    renderWindow->SetReadyForRendering(true);
//    renderWindow->SetForceMaximumHardwareLineWidth(1);
    renderWindow->SetInteractor(m_Interactor);


//        vtkSmartPointer<cellPickInteractorStyle> style = vtkSmartPointer<cellPickInteractorStyle>::New();
//    vtkSmartPointer<propPickInteractorStyle> style = vtkSmartPointer<propPickInteractorStyle>::New();
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    style->SetDefaultRenderer(m_renderer);
//    style->SetMotionFactor(10.0);
//    style->setParent(this);
    m_Interactor->SetInteractorStyle(style);

    m_Interactor->Initialize();
    // Initial camera position
    resetCamera();

//    manager->setRenderer(m_renderer);

//    vtkSmartPointer<vtkHoverCallback> hoverCallback =
//        vtkSmartPointer<vtkHoverCallback>::New();
//      renderWindow->AddObserver(vtkCommand::TimerEvent,hoverCallback);
//      renderWindow->AddObserver(vtkCommand::EndInteractionEvent,hoverCallback);

}

QVTKFBORenderer::~QVTKFBORenderer()
{
    qDeleteAll(pool);
}

QOpenGLFramebufferObject *QVTKFBORenderer::createFramebufferObject(const QSize &size) {
    qDebug()<<cnt<< size;

    QOpenGLFramebufferObject *fbo = framebufferObject();
    if(fbo){
        delete fbo;
    }
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Depth);
    format.setSamples(0);
    format.setMipmap(true);
    fbo = new QOpenGLFramebufferObject(size, format);

//    renderWindow->SetBackLeftBuffer(GL_COLOR_ATTACHMENT0);
//    renderWindow->SetFrontLeftBuffer(GL_COLOR_ATTACHMENT0);
//    renderWindow->SetBackBuffer(GL_COLOR_ATTACHMENT0);
//    renderWindow->SetFrontBuffer(GL_COLOR_ATTACHMENT0);
//    renderWindow->SetSize(fbo->size().width(), fbo->size().height());
//    renderWindow->SetOffScreenRendering(true);
//    renderWindow->Modified();

    return fbo;
}

void QVTKFBORenderer::resetCamera() {
    renderWindow->OpenGLInitState();
    renderWindow->MakeCurrent();
    QOpenGLFunctions::initializeOpenGLFunctions();
    QOpenGLFunctions::glUseProgram(0);

//    m_renderer->ResetCameraClippingRange();
//    m_renderer->GetActiveCamera()->SetViewAngle(30);
//    m_renderer->GetActiveCamera()->SetPosition(0, 0, 1);
//    m_renderer->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);
//    m_renderer->GetActiveCamera()->SetViewUp(0.0, 1, 0.0);

}

void QVTKFBORenderer::initScene() {
    QString msg = QString("OpenGL: %1 (%2)").arg(reinterpret_cast<const char *>(glGetString(GL_VERSION)),
                                                  vtkVersion::GetVTKSourceVersion());
    qDebug() << msg;
    m_fboItem->message(msg);

    m_renderer->SetBackground(9 / 255., 13 / 255., 37 / 255.);
    m_renderer->SetBackground2(9 / 255., 13 / 255., 33 / 255.);
    m_renderer->SetBackgroundAlpha(0.5);
    m_renderer->GradientBackgroundOn();
    connect(m_fboItem, &QVTKFBOItem::openFile, this, &QVTKFBORenderer::addObject);
    connect(m_fboItem, &QVTKFBOItem::cameraOrientationChanged, this, &QVTKFBORenderer::setCamera);

    addObject("axes");
//    addObject("/home/wayne/3d/red_pepper.stl");
//    addObject("/home/wayne/3d/00001-pano.jpg");
//    addObject("/home/wayne/3d/obj/a.stl");
//    addObject("/home/wayne/3d/obj/knot.stl");
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
            eye[1] = -1.0 * mfactor;
            eye[2] = 0;
            top[0] = 0;
            top[1] = 0;
            top[2] = 1.0 * mfactor;
            break;
    }

    m_renderer->GetActiveCamera()->SetPosition(eye);
    m_renderer->GetActiveCamera()->SetFocalPoint(center);
    m_renderer->GetActiveCamera()->SetViewUp(top);
    update();
}

void QVTKFBORenderer::addObject(QString filename)
{
    auto tmp = new QVTKObject3D(QUrl(filename).path());
    ////////////////////////////////////////////
    QThread *job= new QThread;
    tmp->moveToThread(job);
    connect(job, &QThread::started, tmp, &QVTKObject3D::load);
    connect(job, &QThread::finished, job, &QThread::deleteLater);
    connect(tmp, &QVTKObject3D::done, this, &QVTKFBORenderer::onObjectAdded);
    job->start();
}

QVTKObject3D *QVTKFBORenderer::getObject(vtkProp3D *prop) const
{
    return pool.value(prop,nullptr);
}

void QVTKFBORenderer::onObjectAdded(int status, QString name)
{
    qDebug() << status << name << pool.keys();
    QVTKObject3D *tmp = static_cast<QVTKObject3D*>(sender());
    if(tmp){
//        qDebug() << tmp->name();
        pool.insert(tmp->getProp3d(),tmp);
        m_renderer->AddActor(tmp->getProp3d());
        m_renderer->Modified();
        update();
    }
}

void QVTKFBORenderer::handleKey(std::shared_ptr<QEvent> keyevent)
{
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
}

void QVTKFBORenderer::handleMouse(std::shared_ptr<QEvent> mouseevent, vtkSmartPointer<vtkTransform> trans)
{
    QMouseEvent *mouse = static_cast<QMouseEvent *>(mouseevent.get());
//    qDebug() << "point: " << mouse << m_Interactor->GetEventPosition()[0] << " "<< m_Interactor->GetEventPosition()[1];
    renderWindow->GetInteractor()->GetPicker()->Pick(mouse->x(),mouse->y(),0,m_renderer);
    double picked[3];
    picker->GetPickPosition(picked);
    QString ans = QString("picked point: (%1,%2,%3)").arg(picked[0],3).arg(picked[1],3).arg(picked[2],3);

    //                    std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;

    picker->Pick(mouse->x(),mouse->y(),0, renderWindow->GetRenderers()->GetFirstRenderer());
    auto prop = picker->GetProp3D();
    if(prop){
        selected_Prop = prop;
        auto pos = m_renderer->GetActiveCamera()->GetPosition();
        qDebug() << prop->GetClassName()<<prop << getObject(prop)->name();
        qDebug() << ans<<QString("camera position: (%1,%2,%3)").arg(pos[0],3).arg(pos[1],3).arg(pos[2],3);
        ans.append("\t");
        ans.append(QString("picked object (%1)").arg(getObject(prop)->name()));
        if(mouse->type() ==QEvent::MouseButtonDblClick){
            auto o = getObject(selected_Prop);
            o->alignToOrigin();

            static bool flag = true;
            flag = !flag;
            vtkActor *tmp = dynamic_cast<vtkActor*>(selected_Prop);
            if(tmp){
                if(flag){
                    tmp->GetProperty()->SetColor(1,1,1);
                } else {
                    tmp->GetProperty()->SetColor(.5,0,0);
                }
                tmp->GetProperty()->SetAmbient(0.1);
                tmp->GetProperty()->SetDiffuse(0.7);
                tmp->GetProperty()->SetSpecular(0.3);
                tmp->GetProperty()->SetOpacity(0.9);
                if(o->type==4){
                    auto s = tmp->GetScale();
                    tmp->SetScale(*s*.5);
                }
            }
        }
    }
    emit m_fboItem->message(ans);
}

void QVTKFBORenderer::synchronize(QQuickFramebufferObject *item) {
//    qDebug() <<++cnt<<QString(30,'-');
    if (!m_fboItem) {
        m_fboItem = static_cast<QVTKFBOItem *>(item);
        initScene();
        m_fboItem->sceneCompleted();
    }
    if(renderWindow && renderWindow->GetReadyForRendering()) {

//        vtkQuaterniond tmp;
//        animator->InterpolateQuaternion(m_fboItem->m_progress, tmp);

        auto trans = vtkSmartPointer<vtkTransform>::New();
        trans->Identity();
        trans->Translate(m_fboItem->m_position.x(),m_fboItem->m_position.y(),m_fboItem->m_position.z());
        trans->Scale(m_fboItem->m_scale3D,m_fboItem->m_scale3D,m_fboItem->m_scale3D);
//        trans->RotateX(90*m_fboItem->m_progress);

        trans->RotateWXYZ(m_fboItem->m_pose.scalar()
                          ,m_fboItem->m_pose.x()
                          ,m_fboItem->m_pose.y()
                          ,m_fboItem->m_pose.z());

        if(m_fboItem->m_running){
//            m_renderer->GetActiveCamera()->ApplyTransform(trans);

            if(selected_Prop){
                auto pos= selected_Prop->GetPosition();
                qDebug() << "m_fboItem->m_pose = " << m_fboItem->m_pose<<m_fboItem->m_position;
                selected_Prop->AddPosition(trans->GetPosition());
                selected_Prop->SetOrigin(-pos[0],-pos[1],-pos[2]);
                selected_Prop->AddOrientation(trans->GetOrientation());
                selected_Prop->SetScale(trans->GetScale());
            }
        }

        while (!m_fboItem->events.empty()) {
            auto e = m_fboItem->events.takeFirst();
            e->accept();
//            qDebug() <<e.get();
            auto t = e->type();
            if (t == QEvent::MouseButtonPress/*||t==QEvent::MouseButtonRelease*/
                    ||t==QEvent::MouseButtonDblClick/*||t==QEvent::MouseMove*/) {
                handleMouse(e,trans);

            } else if(t==QEvent::KeyPress){
                handleKey(e);
            }
            m_dapter->ProcessEvent(e.get(), renderWindow->GetInteractor());
        }
    }
}

void QVTKFBORenderer::render() {
//    qDebug() <<cnt<<QString(30,'-');

    renderWindow->PushState();
    renderWindow->Start();
    renderWindow->Render();
    renderWindow->PopState();
    m_fboItem->window()->resetOpenGLState();
}


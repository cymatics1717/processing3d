#include "pclobject.hpp"
#include <ostream>
#include <QDebug>
#include <vtkPoints.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyData.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPointData.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkLODActor.h>
#include <vtkVersion.h>
#include <vtkCamera.h>
#include <QThread>
#include <QTime>
#include <src/vtk/proppickinteractorstyle.hpp>

struct node {
    float point[3];
    uint8_t color[4];
    friend std::ostream& operator<<(std::ostream& out, const node& n){
        out << "[point:("<<n.point[0]<<","<<n.point[1]<<","<<n.point[2]<<"),color=("
            << int(n.color[0])<<","<<int(n.color[1])<<","<<int(n.color[2])<<")"<<std::endl;
        return out;
    }
};

PCLObject::PCLObject(QObject *parent) : OpenGLObject(parent),cloud (new pcl::PointCloud<pcl::PointXYZRGB>)
  ,m_renderWindow(vtkSmartPointer<vtkExternalOpenGLRenderWindow>::New())
  ,m_renderer(vtkSmartPointer<vtkRenderer>::New())
  ,m_interactor(vtkSmartPointer<QVTKInteractor>::New())
  ,m_dapter(new QVTKInteractorAdapter(this))
{
    qDebug() << vtkVersion::GetVTKSourceVersion();
}

PCLObject::~PCLObject()
{

}

void PCLObject::initialize()
{
    m_renderWindow->AddRenderer(m_renderer);
    m_renderWindow->OpenGLInitContext();
    m_renderWindow->SetOffScreenRendering(true);
    m_renderWindow->SetInteractor(m_interactor);

//    vtkSmartPointer<propPickInteractorStyle> style = vtkSmartPointer<propPickInteractorStyle>::New();
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    style->SetDefaultRenderer(m_renderer);
//    style->SetMotionFactor(10.0);
//    style->setParent(this);
    m_interactor->SetInteractorStyle(style);
    m_interactor->Initialize();

    m_renderWindow->OpenGLInitState();
    m_renderWindow->MakeCurrent();
//    QOpenGLFunctions::initializeOpenGLFunctions();
    glUseProgram(0);

    m_renderer->ResetCameraClippingRange();
//    m_renderer->GetActiveCamera()->SetPosition(0, 0, 1);
//    m_renderer->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);
//    m_renderer->GetActiveCamera()->SetViewUp(0.0, 1, 0.0);
}

void PCLObject::render()
{
    m_renderWindow->PushState();
    m_renderWindow->Render();
    m_renderWindow->PopState();
}

void PCLObject::mousePressEvent(std::shared_ptr<QEvent> event)
{
    qDebug() << event.get();
    m_dapter->ProcessEvent(event.get(), m_interactor);
}

void PCLObject::mouseMoveEvent(std::shared_ptr<QEvent> event)
{
    qDebug() << event.get();
    m_dapter->ProcessEvent(event.get(), m_interactor);
}

void PCLObject::mouseReleaseEvent(std::shared_ptr<QEvent> event)
{
    qDebug() << event.get();
    m_dapter->ProcessEvent(event.get(), m_interactor);
}

void PCLObject::mouseDoubleClickEvent(std::shared_ptr<QEvent> event)
{
    qDebug() << event.get();
    m_dapter->ProcessEvent(event.get(), m_interactor);
}

void PCLObject::keyPressEvent(std::shared_ptr<QEvent> event)
{
    qDebug() << event.get();
    auto key_event = static_cast<QKeyEvent*>(event.get());

    m_dapter->ProcessEvent(event.get(), m_interactor);
    auto camera = m_renderer->GetActiveCamera();
    auto pos = camera->GetPosition();
    if(key_event->key()==Qt::Key_Left){
        camera->SetPosition(pos[0]-1,pos[1], pos[2]);
    } else  if(key_event->key()==Qt::Key_Right){
        camera->SetPosition(pos[0]+1,pos[1], pos[2]);
    } else if(key_event->key()==Qt::Key_Up){
        camera->SetPosition(pos[0],pos[1]+1, pos[2]);
    } else if(key_event->key()==Qt::Key_Down){
        camera->SetPosition(pos[0],pos[1]-1, pos[2]);
    } else if(key_event->key()==Qt::Key_PageUp){
        camera->SetPosition(pos[0],pos[1], pos[2]+1);
    } else if(key_event->key()==Qt::Key_PageDown){
        camera->SetPosition(pos[0],pos[1], pos[2]-1);
    }
}

void PCLObject::keyReleaseEvent(std::shared_ptr<QEvent> event)
{
    qDebug() << event.get();
    m_dapter->ProcessEvent(event.get(), m_interactor);
}

void PCLObject::wheelEvent(std::shared_ptr<QEvent> event)
{
    qDebug() << event.get();
    m_dapter->ProcessEvent(event.get(), m_interactor);
}

void PCLObject::touchEvent(std::shared_ptr<QEvent> event)
{
    qDebug() << event.get();
    m_dapter->ProcessEvent(event.get(), m_interactor);
}

void PCLObject::loadFile(const QString &filename)
{
    qDebug() << QString(30,'*') << cloudfile;
    cloudfile = filename;
    loadAsync();
//    QThread *job= new QThread;
//    this->moveToThread(job);
//    connect(job, &QThread::started, this, &PCLObject::loadAsync);
//    connect(job, &QThread::finished, job, &QObject::deleteLater);
//    job->start();
//    qDebug() << QString(30,'*')<< cloudfile;

}

void PCLObject::loadAsync()
{
    QDateTime  start = QDateTime::currentDateTime();
    qDebug() << QString(30,'*') << start.toString() << cloudfile;
    QString source = QUrl(cloudfile).path();
    if(source.at(0)!=':'&&source.contains(':')){
        source.remove(0,1);
    }
    if(pcl::io::load<pcl::PointXYZRGB>(source.toStdString(), *cloud) == -1) // load the file
    {
      pcl::console::print_error ("Couldn't read file %s!\n", source.toStdString().c_str());
      return;
    }

    vtkSmartPointer<vtkPoints> pointSource =  vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    for(size_t i=0;i<cloud->size();++i){
        pointSource->InsertNextPoint(cloud->points.at(i).x,cloud->points.at(i).y,cloud->points.at(i).z);
        colors->InsertNextTuple3(cloud->points.at(i).r,cloud->points.at(i).g,cloud->points.at(i).b);
    }

    vtkSmartPointer<vtkPolyData> pointsPolydata =  vtkSmartPointer<vtkPolyData>::New();
    pointsPolydata->SetPoints(pointSource);
    vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter =  vtkSmartPointer<vtkVertexGlyphFilter>::New();
    vertexFilter->AddInputData(pointsPolydata);
    vertexFilter->Update();

    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->ShallowCopy(vertexFilter->GetOutput());
    polydata->GetPointData()->SetScalars(colors);


    auto b = polydata->GetBounds();
    auto c = polydata->GetCenter();
    qDebug() << b[0]<< b[1]<< b[2]<< b[3]<< b[4]<< b[5];
    qDebug() << c[0]<< c[1]<< c[2];

    auto translation = vtkSmartPointer<vtkTransform>::New();
    translation->Translate(-c[0],-c[1],-c[2]);

    auto m_modelFilterTranslate = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    m_modelFilterTranslate->SetInputData(polydata);
    m_modelFilterTranslate->SetTransform(translation);
    m_modelFilterTranslate->Update();
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New ();
    mapper->SetInputConnection(m_modelFilterTranslate->GetOutputPort());
//    mapper->SetColorModeToMapScalars();
//    mapper->SetScalarModeToUsePointData();
//    mapper->ScalarVisibilityOff ();

    vtkSmartPointer<vtkLODActor> actor = vtkSmartPointer<vtkLODActor>::New ();
//    actor->GetProperty()->SetColor(0,1,0);
    actor->GetProperty()->SetPointSize(2);
    actor->SetMapper (mapper);

    m_renderer->AddActor(actor);
    QDateTime  end = QDateTime::currentDateTime();
    qDebug() <<start.msecsTo(end)+"ms"<<QString(30,'*') << end.toString() << cloudfile;
}

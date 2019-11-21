#include "qvtkobject3d.hpp"

#include <QFileInfo>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>
#include <vtkTexturedSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransformTextureCoords.h>
#include <vtkProperty.h>
#include <QDebug>
#include <vtkOBJReader.h>
#include <vtkSTLReader.h>
#include <vtkPLYReader.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkAxesActor.h>
#include <vtkTextActor.h>
#include <vtkCaptionActor2D.h>

#include <pcl/console/parse.h>
#include <pcl/console/print.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/io/auto_io.h>
#include <pcl/keypoints/harris_3d.h>
#include <pcl/conversions.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/common/angles.h>

QVTKObject3D::QVTKObject3D(QString name, QObject *parent) :
    QObject(parent),m_name(name),oorigin({0,0,0})
{
    qDebug() << name;
}

QVTKObject3D::~QVTKObject3D()
{
    qDebug() <<m_name;
}

void QVTKObject3D::load()
{
    QFileInfo info(m_name);
    auto suffix = info.suffix().toLower();
    int status = -1;
    if(suffix.isEmpty()){
        status = loadData();
    } else if(suffix =="obj" || suffix =="stl"/*||suffix == "ply"*/) {
        status = loadMesh();
    } else if(suffix == "ply" || suffix == "pcd"){
        status = loadCloud();
    } else {
        status = loadImage();
    }
    done(status,m_name);
}

int QVTKObject3D::loadMesh()
{
    QFileInfo info(m_name);
    auto suffix = info.suffix().toLower();

    vtkSmartPointer<vtkAbstractPolyDataReader> reader = nullptr;
    vtkSmartPointer<vtkPolyData> inputData = nullptr;
    if (suffix == "obj") {
        reader = vtkSmartPointer<vtkOBJReader>::New();
    } else if (suffix == "stl") {
        reader = vtkSmartPointer<vtkSTLReader>::New();
    } else if (suffix == "ply") {
        reader = vtkSmartPointer<vtkPLYReader>::New();
    }
    if(reader){
        reader->SetFileName(m_name.toStdString().c_str());
        reader->Update();
        inputData = reader->GetOutput();
    }
    if(inputData){
        auto translation = vtkSmartPointer<vtkTransform>::New();
        auto c = inputData->GetCenter();
        qDebug() << c[0]<< c[1]<< c[2];
        oorigin.setX(c[0]);
        oorigin.setY(c[1]);
        oorigin.setZ(c[2]);

        double n=1;
        translation->Scale(n,n,n);

        auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter->SetInputData(inputData);
        transformFilter->SetTransform(translation);
        transformFilter->Update();

        vtkSmartPointer<vtkPolyDataMapper> m_modelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        m_modelMapper->SetInputConnection(transformFilter->GetOutputPort());
        m_modelMapper->ScalarVisibilityOff();

        // Model Actor
        auto actor = vtkSmartPointer<vtkLODActor>::New();
        actor->SetMapper(m_modelMapper);
        actor->GetProperty()->SetInterpolationToFlat();

        actor->GetProperty()->SetAmbient(0.1);
        actor->GetProperty()->SetDiffuse(0.7);
        actor->GetProperty()->SetSpecular(0.3);
        actor->GetProperty()->SetOpacity(0.6);
        actor->SetPosition(0.0, 0.0, 0.0);
        actor->SetPickable(true);
        prop = actor;
        return 0;
    }
    return 1;
}

int QVTKObject3D::loadData()
{
    if (m_name == "axes") {
        // Axes
        vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
        axes->SetXAxisLabelText("XX");
        axes->SetYAxisLabelText("YY");
        axes->SetZAxisLabelText("ZZ");
        double axes_length = 10;
        int16_t axes_label_font_size = 1;
        axes->SetTotalLength(axes_length, axes_length, axes_length);
        axes->GetXAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
        axes->GetYAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
        axes->GetZAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
        axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(axes_label_font_size);
        axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(axes_label_font_size);
        axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(axes_label_font_size);
        axes->SetPickable(true);

        prop = axes;

        return 0;
    }
    return 2;

}

int QVTKObject3D::loadCloud()
{
    QFileInfo info(m_name);
    auto suffix = info.suffix().toLower();

    using pointType = pcl::PointXYZRGB;
    pcl::PointCloud<pointType>::Ptr cloud(new pcl::PointCloud<pointType>);

    int res = pcl::io::load<pointType>(m_name.toStdString(), *cloud);
//    int res = pcl::PLYReader::read(source.toStdString(), *cloud);
    if(res == -1) // load the file
    {
        pcl::console::print_error ("Couldn't read file %s!\n", m_name.toStdString().c_str());
        return loadMesh();
    }

    vtkSmartPointer<vtkPoints> pointSource =  vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);

    for(size_t i=0;i<cloud->size();++i){
        pointSource->InsertNextPoint(cloud->points.at(i).x,cloud->points.at(i).y,cloud->points.at(i).z);
        colors->InsertNextTuple3(cloud->points.at(i).r,cloud->points.at(i).g,cloud->points.at(i).b);
//        qDebug() << cloud->points.at(i).r <<cloud->points.at(i).g<<cloud->points.at(i).b<<cloud->points.at(i).a;
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
    oorigin.setX(c[0]);
    oorigin.setY(c[1]);
    oorigin.setZ(c[2]);

    auto translation = vtkSmartPointer<vtkTransform>::New();
    translation->Translate(0,0,0);
//    translation->Translate(-c[0],-c[1],-c[2]);

    auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInputData(polydata);
    transformFilter->SetTransform(translation);
    transformFilter->Update();
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(transformFilter->GetOutputPort());
//    mapper->SetColorModeToMapScalars();
//    mapper->SetScalarModeToUsePointData();
//    mapper->ScalarVisibilityOff ();

    auto actor = vtkSmartPointer<vtkLODActor>::New();
//    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New ();
//    actor->GetProperty()->SetColor(0,1,0);
    actor->SetMapper (mapper);
    actor->GetProperty()->SetPointSize(2.5);
    prop = actor;
    return 0;
}

int QVTKObject3D::loadImage()
{
    std::string stdstr = m_name.toStdString();
    vtkSmartPointer<vtkImageReader2Factory> readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkSmartPointer<vtkImageReader2> imageReader = readerFactory->CreateImageReader2(stdstr.c_str());

    if (imageReader) {
        imageReader->SetFileName(stdstr.c_str());
        auto source = vtkSmartPointer<vtkTexturedSphereSource>::New();
        source->SetRadius(1000);
        source->SetPhiResolution(20);
        source->SetThetaResolution(20);

        auto texture = vtkSmartPointer<vtkTexture>::New();
        texture->SetInputConnection(imageReader->GetOutputPort());

        auto translation = vtkSmartPointer<vtkTransform>::New();
        translation->Translate(0,0,0);

        auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter->SetInputConnection(source->GetOutputPort());
        transformFilter->SetTransform(translation);
//        transformFilter->Update();


        //    double translate[3] = {0, 0, 0};
        auto transformTexture = vtkSmartPointer<vtkTransformTextureCoords>::New();
        transformTexture->SetInputConnection(transformFilter->GetOutputPort());
        //    transformTexture->SetPosition(translate);
        transformTexture->SetFlipR(false);
        transformTexture->SetFlipS(false);
        transformTexture->SetFlipT(true);

        auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(transformTexture->GetOutputPort());

        auto actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->SetTexture(texture);
        prop = actor;
        return 0;
    }
    return 3;
}

void QVTKObject3D::alignToOrigin()
{
    prop->SetPosition(-oorigin.x(),-oorigin.y(),-oorigin.z());
}

void QVTKObject3D::setColor(QColor color)
{
    vtkActor *tmp = dynamic_cast<vtkActor*>(prop.Get());
    if(tmp){
        tmp->GetProperty()->SetColor(color.red(),color.green(),color.blue());
        tmp->Modified();
    }
}

void QVTKObject3D::applyTransform(vtkSmartPointer<vtkTransform> t)
{
    prop->SetUserTransform(t);
}

vtkSmartPointer<vtkProp3D> QVTKObject3D::getProp3d() const
{
    return prop;
}

QString QVTKObject3D::name() const
{
    return m_name;
}

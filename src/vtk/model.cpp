#include "model.hpp"

#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkPolyDataReader.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkVertexGlyphFilter.h>

#include <vtkOBJReader.h>
#include <vtkSTLReader.h>
#include <QDebug>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkProperty.h>
#include <vtkDICOMImageReader.h>
#include <QFileInfo>
#include <vtkImageProperty.h>
#include <vtkOBJReader.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkSTLReader.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageActor.h>
#include <vtkImageReader2.h>
#include <vtkImageData.h>
#include <vtkVolume.h>
#include <vtkNamedColors.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkStructuredPointsReader.h>
#include <vtkColorTransferFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkInteractorObserver.h>

#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkInteractorStyleImage.h>
#include <vtkExternalOpenGLRenderWindow.h>
#include <QVTKInteractor.h>
#include "modelmanager.hpp"
#include <vtkVertex.h>

#include <vtkPointSource.h>
#include <vtkPointSet.h>
#include <vtkPLYReader.h>
#include <vtkFloatArray.h>
#include <vtkDataSetMapper.h>
#include <vtkLODActor.h>
#include <vtkTexture.h>
#include <vtkTexturedSphereSource.h>
#include <vtkTextureMapToSphere.h>
#include <vtkTransformTextureCoords.h>
#include <QUrl>


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

Model::Model(QString fn, modelManager *manager, QObject *parent): QObject(parent),filename(fn)
{
    this->manager = manager;
}

Model::~Model()
{
    qDebug() << filename <<"removed";
}

QString Model::source() const
{
    return filename;
}

void Model::load()
{
//    loadPointSource();
//    return;
    QFileInfo info(filename);
    auto suffix = info.suffix().toLower();
    int result = 0;
    if(!suffix.isEmpty()){
        if(suffix =="obj" || suffix =="stl") {
            result = loadMesh();
        } else if(suffix == "ply" || suffix == "pcd"){
            result = loadCloud();
        } else {
//            loadImage();
            result = loadPano();
        }
    } else {
        result = loadPrimitive();
    }

    done(filename,result);
}


int Model::loadCloud()
{
    qDebug() << "******************: "<< filename;
    QString source = QUrl(filename).path();
//    if(source.at(0)!=':'&&source.contains(':')){
//        source.remove(0,1);
//    }
    qDebug() << "******************: "<< filename;
    using pointType = pcl::PointXYZRGB;
    pcl::PointCloud<pointType>::Ptr cloud(new pcl::PointCloud<pointType>);

    int res = pcl::io::load<pointType>(source.toStdString(), *cloud);
//    int res = pcl::PLYReader::read(source.toStdString(), *cloud);
    qDebug() <<source<<filename<< res;
    if(res == -1) // load the file
    {
        pcl::console::print_error ("Couldn't read file %s!\n", source.toStdString().c_str());
        loadMesh();
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

//    vtkSmartPointer<vtkLODActor> actor = vtkSmartPointer<vtkLODActor>::New ();
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New ();
//    actor->GetProperty()->SetColor(0,1,0);
    actor->SetMapper (mapper);
    actor->GetProperty()->SetPointSize(2.5);
    manager->getRenderer()->AddActor(actor);
    manager->insertModel(actor, this);
}

int Model::loadCloudPdal()
{

}

int Model::loadPrimitive()
{
    if (filename == "axes") {
        // Axes
        vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
        axes->SetXAxisLabelText("XX");
        axes->SetYAxisLabelText("YY");
        axes->SetZAxisLabelText("ZZ");
        double axes_length = 20.0;
        int16_t axes_label_font_size = 10;
        axes->SetTotalLength(axes_length, axes_length, axes_length);
        axes->GetXAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
        axes->GetYAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
        axes->GetZAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
        axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(axes_label_font_size);
        axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(axes_label_font_size);
        axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(axes_label_font_size);
        axes->SetPickable(true);
        //        manager->getRenderer()->AddActor(axes);
        manager->getRenderer()->AddViewProp(axes);
        prop = axes;
        manager->insertModel(prop.Get(), this);
    }
    return 0;
}

int Model::loadPano()
{
    std::string stdstr = filename.toStdString();
    vtkSmartPointer<vtkImageReader2Factory> readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkSmartPointer<vtkImageReader2> imageReader = readerFactory->CreateImageReader2(stdstr.c_str());

    if (imageReader) {
        imageReader->SetFileName(stdstr.c_str());
        static vtkSmartPointer<vtkTexturedSphereSource> source = nullptr;
        if(source == nullptr){
            source = vtkSmartPointer<vtkTexturedSphereSource>::New();
            source->SetRadius(1000);
            source->SetPhiResolution(20);
            source->SetThetaResolution(20);
        }

        auto texture = vtkSmartPointer<vtkTexture>::New();
        texture->SetInputConnection(imageReader->GetOutputPort());

        //    auto texturemap = vtkSmartPointer<vtkTextureMapToSphere>::New();
        //    texturemap->SetInputConnection(source->GetOutputPort());
        //    texturemap->SetCenter(0.5, 0.5, 0.5);
        //    texturemap->SetPreventSeam(true);
        //    texturemap->SetAutomaticSphereGeneration(true);

        //    double translate[3] = {0, 0, 0};
        auto transformTexture = vtkSmartPointer<vtkTransformTextureCoords>::New();
        transformTexture->SetInputConnection(source->GetOutputPort());
        //    transformTexture->SetPosition(translate);
        transformTexture->SetFlipR(true);
        transformTexture->SetFlipS(false);
        transformTexture->SetFlipT(true);

        auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(transformTexture->GetOutputPort());

        auto actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->SetTexture(texture);
        manager->getRenderer()->AddViewProp(actor);
        //    prop = actor;
        manager->insertModel(prop.Get(), this);
    }
}

int Model::loadImage()
{
    QFileInfo info(filename);
    std::string stdstr = filename.toStdString();

    vtkSmartPointer<vtkAlgorithm> reader = nullptr;
    vtkSmartPointer<vtkImageData> input = nullptr;

    if(info.suffix()=="dcm"){
        vtkSmartPointer<vtkDICOMImageReader> dicomReader = vtkDICOMImageReader::New();
        //        dicomReader->SetDirectoryName(filename.c_str());
        dicomReader->SetFileName(stdstr.c_str());
        dicomReader->Update();
        input = dicomReader->GetOutput();
        reader = dicomReader;
    } else {
        vtkSmartPointer<vtkImageReader2Factory> readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
        vtkSmartPointer<vtkImageReader2> imageReader = readerFactory->CreateImageReader2(stdstr.c_str());
        if(imageReader){
            imageReader->SetFileName(stdstr.c_str());
            imageReader->Update();
            input = imageReader->GetOutput();
            reader = imageReader;
        }
    }

    if(reader){
        // Create an actor
        vtkSmartPointer<vtkImageActor> actor_img = vtkSmartPointer<vtkImageActor>::New();
        actor_img->SetInputData(input);
        actor_img->SetOpacity(.5);
        actor_img->SetPickable(true);
        int *a =input->GetDimensions();
        qDebug() << a[0] << a[1];
        //          actor_img->SetOrigin(a[0]/2,a[1]/2,0);
        actor_img->SetPosition(-a[0]/2,-a[1]/2,0);

        prop = actor_img;
        manager->insertModel(prop.Get(),this);
        manager->getRenderer()->AddActor(actor_img);
        return 0;
    } else {
        return -1;
    }
}

int Model::loadMesh()
{
    QFileInfo info(filename);
    qDebug() << info;
    vtkSmartPointer<vtkPolyData> inputData = nullptr;
    if (info.suffix().toLower() == "obj") {
        // Read OBJ file
        auto reader = vtkSmartPointer<vtkOBJReader>::New();
        reader->SetFileName ( filename.toStdString().c_str());
        reader->Update();
        inputData = reader->GetOutput();
    } else if (info.suffix().toLower() == "stl") {
        // Read STL file
            qDebug() << info;
        auto reader = vtkSmartPointer<vtkSTLReader>::New();
        reader->SetFileName ( filename.toStdString().c_str());
        reader->Update();
        inputData = reader->GetOutput();
    } else if (info.suffix().toLower() == "ply") {
        // Read PLY file
        auto reader = vtkSmartPointer<vtkPLYReader>::New();
        reader->SetFileName ( filename.toStdString().c_str());
        reader->Update();
        inputData = reader->GetOutput();
    }

    qDebug() << info;
    if(inputData){
        qDebug() << info;

        auto translation = vtkSmartPointer<vtkTransform>::New();
        double n=1;
        translation->Scale(n,n,n);

        auto m_modelFilterTranslate = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        m_modelFilterTranslate->SetInputData(inputData);
        m_modelFilterTranslate->SetTransform(translation);
        m_modelFilterTranslate->Update();

        // Model Mapper
        vtkSmartPointer<vtkPolyDataMapper> m_modelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        m_modelMapper->SetInputConnection(m_modelFilterTranslate->GetOutputPort());
        m_modelMapper->ScalarVisibilityOff();

        // Model Actor
        auto m_modelActor = vtkSmartPointer<vtkLODActor>::New();
        m_modelActor->SetMapper(m_modelMapper);
        m_modelActor->GetProperty()->SetInterpolationToFlat();

        m_modelActor->GetProperty()->SetAmbient(0.1);
        m_modelActor->GetProperty()->SetDiffuse(0.7);
        m_modelActor->GetProperty()->SetSpecular(0.3);
        m_modelActor->GetProperty()->SetOpacity(0.6);
        m_modelActor->SetPosition(0.0, 0.0, 0.0);
        m_modelActor->SetPickable(true);
        prop = m_modelActor;
        manager->insertModel(prop.Get(),this);
//        manager->getRenderer()->AddActor(m_modelActor);
        manager->getRenderer()->AddViewProp(m_modelActor);

        return 0;
    } else {
        return -1;
    }
}

#include "proppickinteractorstyle.hpp"
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkPickingManager.h>
#include <vtkRenderer.h>
#include <QDebug>
#include <vtkCellPicker.h>

vtkStandardNewMacro(propPickInteractorStyle);

propPickInteractorStyle::propPickInteractorStyle():parent(nullptr),NumberOfClicks(0), ResetPixelDistance(5)
{
    PreviousPosition[0] = 0;
    PreviousPosition[1] = 0;

    LastPickedActor = nullptr;
    LastPickedProperty = vtkProperty::New();
//    cell_picker = vtkSmartPointer<vtkCellPicker>::New();
//    cell_picker->SetTolerance(0.0);

    prop_picker = vtkSmartPointer<vtkPropPicker>::New();
}

propPickInteractorStyle::~propPickInteractorStyle()
{
    LastPickedProperty->Delete();
}

void propPickInteractorStyle::OnLeftButtonDown()
{
    NumberOfClicks++;
    int pickPosition[2];
    GetInteractor()->GetEventPosition(pickPosition);

    int xdist = pickPosition[0] - PreviousPosition[0];
    int ydist = pickPosition[1] - PreviousPosition[1];

    PreviousPosition[0] = pickPosition[0];
    PreviousPosition[1] = pickPosition[1];

    int moveDistance = (int) sqrt((double) (xdist * xdist + ydist * ydist));

    // Reset numClicks - If mouse moved further than resetPixelDistance
    if (moveDistance > ResetPixelDistance) {
        NumberOfClicks = 1;
    }


    vtkRenderer *renderer = GetDefaultRenderer();
    vtkSmartPointer<vtkPropPicker>  picker = vtkSmartPointer<vtkPropPicker>::New();
    picker->Pick(pickPosition[0], renderer->GetSize()[1]+pickPosition[1],0, renderer);

    if (NumberOfClicks == 2) {
//        std::cout << "Double clicked." <<pickPosition[0]<<","<<pickPosition[1]<< std::endl;
        NumberOfClicks = 0;

//        double picked[3];
//        picker->GetPickPosition(picked);
//        std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;
//        parent->updateCamera(picked[0],picked[1],picked[2]);


    } else {

        //    qDebug() <<picker->GetProp3D() << picker->GetActor()/* << filename*/;
        // If we picked something before, reset its property
        if (LastPickedActor)
        {
            LastPickedActor->GetProperty()->DeepCopy(LastPickedProperty);
        }

        LastPickedActor = picker->GetActor();
        if (LastPickedActor)
        {

            qDebug() << picker->GetProp3D()->GetClassName()<<picker->GetProp3D();
            // Save the property of the picked actor so that we can
            // restore it next time
            LastPickedProperty->DeepCopy(LastPickedActor->GetProperty());
            // Highlight the picked actor by changing its properties
            LastPickedActor->GetProperty()->SetColor(0., 0.5, 1);
            LastPickedActor->GetProperty()->SetDiffuse(1.0);
            LastPickedActor->GetProperty()->SetSpecular(0.0);

            if(parent){
                QString filename = parent->getObject(picker->GetProp3D())->name();
                parent->selectedModel(filename);
            }

        } else if(picker->GetProp3D()){

            if(parent){
                QString filename = parent->getObject(picker->GetProp3D())->name();
                qDebug() << picker->GetProp3D()->GetClassName()<<picker->GetProp3D()  << filename;
                parent->selectedModel(filename);
            }
        }
    }

    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void propPickInteractorStyle::OnRightButtonDown()
{
    int* clickPos = GetInteractor()->GetEventPosition();

    vtkRenderer *renderer = GetDefaultRenderer();
    vtkSmartPointer<vtkPropPicker>  picker = vtkSmartPointer<vtkPropPicker>::New();
    picker->Pick(clickPos[0], renderer->GetSize()[1]+clickPos[1],0, renderer);


    // If we picked something before, reset its property
    if (LastPickedActor)
    {
        LastPickedActor->GetProperty()->DeepCopy(LastPickedProperty);
    }
    LastPickedActor = picker->GetActor();
    if (LastPickedActor)
    {
//        QString filename = parent->manager->getModelName(picker->GetProp3D(),"none");
        qDebug() << picker->GetProp3D()->GetClassName()<<picker->GetProp3D()  /*<< filename*/;
        // Save the property of the picked actor so that we can
        // restore it next time
        LastPickedProperty->DeepCopy(LastPickedActor->GetProperty());
        // Highlight the picked actor by changing its properties
        LastPickedActor->GetProperty()->SetColor(0., 0.5, .8);
        LastPickedActor->GetProperty()->SetDiffuse(1.0);
        LastPickedActor->GetProperty()->SetSpecular(0.0);

//        emit parent->selectedModel(filename);
    }

    vtkInteractorStyleTrackballCamera::OnRightButtonDown();
}

void propPickInteractorStyle::setParent(QVTKFBORenderer *parent)
{
    this->parent = parent;
}


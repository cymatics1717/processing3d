#ifndef propPickInteractorStyle_HPP
#define propPickInteractorStyle_HPP

#include "qvtkfborenderer.hpp"

#include <QMap>
#include <vtkActor.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>

class propPickInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static propPickInteractorStyle* New();
    vtkTypeMacro(propPickInteractorStyle, vtkInteractorStyleTrackballCamera)

    void OnLeftButtonDown() override;
    void OnRightButtonDown() override;
//    void OnChar() override;
//    void OnMouseMove() override;

    void setParent(QVTKFBORenderer *parent);
protected:
    propPickInteractorStyle();
    ~propPickInteractorStyle() override;
private:
    vtkActor    *LastPickedActor;
    vtkProperty *LastPickedProperty;
    vtkSmartPointer<vtkPropPicker>  prop_picker;

    QVTKFBORenderer *parent;
    unsigned int NumberOfClicks;
    int PreviousPosition[2];
    int ResetPixelDistance;
};


#endif // propPickInteractorStyle_HPP

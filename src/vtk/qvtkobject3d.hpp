#ifndef QVTKOBJECT3D_HPP
#define QVTKOBJECT3D_HPP

#include <QObject>
#include <vtkTransform.h>
#include <vtkActor.h>
#include <vtkTransformPolyDataFilter.h>
#include <QVector3D>
#include <QColor>

class QVTKObject3D : public QObject
{
    Q_OBJECT
public:
    explicit QVTKObject3D(QString name, QObject *parent = nullptr);
    ~QVTKObject3D();
signals:
    void done(int status,QString name);

public slots:
    void load();
    int loadMesh();
    int loadData();
    int loadCloud();
    int loadImage();

    void alignToOrigin();
    void initObjectOriginalData(vtkSmartPointer<vtkPolyData> inputData);
    void setColor(QColor color);
    void applyTransform(vtkSmartPointer<vtkTransform> t);
    vtkSmartPointer<vtkProp3D> getProp3d() const;
    QString name() const;
private:
    QString m_name;

    vtkSmartPointer<vtkProp3D> prop;

public:
    int type;
    QVector3D center;
    QVector<double> boundingBox;
    QVector3D position;
    QVector3D oritation;
};

#endif // QVTKOBJECT3D_HPP

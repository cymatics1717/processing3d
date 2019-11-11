#ifndef PCLOBJECT_HPP
#define PCLOBJECT_HPP

#include "openglobject.hpp"

#include <pcl/console/parse.h>
#include <pcl/console/print.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/io/auto_io.h>
#include <pcl/keypoints/harris_3d.h>

#include <QVTKInteractor.h>
#include <QVTKInteractorAdapter.h>
#include <vtkExternalOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

class PCLObject : public OpenGLObject
{
    Q_OBJECT
public:
    explicit PCLObject(QObject *parent = nullptr);
    ~PCLObject()override;

    void initialize() override;
    void render() override;
    void mousePressEvent(std::shared_ptr<QEvent> event) override;
    void mouseMoveEvent(std::shared_ptr<QEvent> event) override;
    void mouseReleaseEvent(std::shared_ptr<QEvent> event) override;
    void mouseDoubleClickEvent(std::shared_ptr<QEvent> event) override;
    void keyPressEvent(std::shared_ptr<QEvent> event) override;
    void keyReleaseEvent(std::shared_ptr<QEvent> event) override;
    void wheelEvent(std::shared_ptr<QEvent> event) override;
    void touchEvent(std::shared_ptr<QEvent> event) override;


signals:

public slots:
    void loadFile(const QString &filename);
    void loadAsync();
private:
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud;

    vtkSmartPointer<vtkExternalOpenGLRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<QVTKInteractor> m_interactor;
    QVTKInteractorAdapter* m_dapter;

    QString cloudfile;

};

#endif // PCLOBJECT_HPP

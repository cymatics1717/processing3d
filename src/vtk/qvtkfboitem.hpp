#ifndef QVTKFBOITEM_HPP
#define QVTKFBOITEM_HPP

#include <QQuickFramebufferObject>
#include <QVector3D>
#include <memory>
#include <QQuaternion>

class QVTKFBORenderer;
class QVTKFBOItem : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    Q_ENUMS(Orientation)
    enum class Orientation {
        TopView, BottomView, FrontView,BackView,LeftView,RightView,ISOBackView,ISOFrontView,OriginView
    };

    Q_PROPERTY(Orientation camera_Orientation MEMBER m_camera_Orientation NOTIFY cameraOrientationChanged)
    Q_PROPERTY(QQuaternion pose MEMBER m_pose NOTIFY poseChanged)

    QVTKFBOItem();
    QQuickFramebufferObject::Renderer *createRenderer() const override;

    QList<std::shared_ptr<QEvent>> events;
    QQuaternion m_pose;

public slots:
signals:
    void openFile(QString filename);
    void selectedModel(QString modelFileName);
    void hoverEvent(int type,QPoint oldPos, QPoint newPos);

    void message(QString msg);
    void toggleWindow();
    void toggleShow();
    void stepback();
    void clicked();
    void cameraOrientationChanged(Orientation oritation);
    void poseChanged(QQuaternion pose);

protected:
    bool eventFilter(QObject *object, QEvent *evt) override;

public slots:
    void handleEvent(std::shared_ptr<QEvent> event);

private:
    QVTKFBORenderer *renderer;
    Orientation m_camera_Orientation;
};

#endif // QVTKFBOITEM_HPP

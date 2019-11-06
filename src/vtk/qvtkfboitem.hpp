#ifndef QVTKFBOITEM_HPP
#define QVTKFBOITEM_HPP

#include <QQuickFramebufferObject>
#include <QVector3D>
#include <memory>

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

    QVTKFBOItem();
    QQuickFramebufferObject::Renderer *createRenderer() const override;

    QList<std::shared_ptr<QEvent>> events;

public slots:
signals:
    void openFile(QString filename);
    void selectedModel(QString modelFileName);
    void hoverEvent(int type,QPoint oldPos, QPoint newPos);

    void message(QString msg);
    void toggleWindow();
    void toggleShow();
    void stepback();
    void cameraOrientationChanged(Orientation oritation);

protected:
    bool eventFilter(QObject *object, QEvent *evt) override;

public slots:
    void handleEvent(std::shared_ptr<QEvent> event);

private:
    QVTKFBORenderer *renderer;
    Orientation m_camera_Orientation;

};

#endif // QVTKFBOITEM_HPP

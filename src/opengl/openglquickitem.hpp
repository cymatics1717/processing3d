#ifndef OpenGLQuickItem_HPP
#define OpenGLQuickItem_HPP

#include <QQuickFramebufferObject>
#include <memory>

class OpenGLQuickItem : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QStringList images MEMBER m_images NOTIFY imagesChanged)
//    Q_PROPERTY(qreal spacing MEMBER m_spacing NOTIFY spacingChanged)
//    Q_PROPERTY(QString text MEMBER m_text NOTIFY textChanged)
    Q_ENUMS(Orientation)
    enum class Orientation {
        TopView, BottomView, FrontView,BackView,LeftView,RightView,ISOBackView,ISOFrontView,OriginView
    };

    Q_PROPERTY(Orientation camera_Orientation MEMBER m_camera_Orientation NOTIFY cameraOrientationChanged)
    OpenGLQuickItem();
    Renderer *createRenderer() const override;

    QList<std::shared_ptr<QEvent>> events;

//public slots:
signals:
    void openFile(const QString &file);
    void openFolder(const QString &file);
    void imagesChanged();
    void toggleWindow();
    void toggleShow();
    void stepback();
    void blend(float);
//    void incomingMessage(QString msg);
    void setSwitchType(int stype);
    int setPhotoIndex(int idx);

    void cameraOrientationChanged(Orientation oritation);

public slots:
    void onOpenFolder(const QString &file);

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void handleEvent(std::shared_ptr<QEvent> event);
private:
    QStringList m_images;
    Orientation m_camera_Orientation;
};

#endif // OpenGLQuickItem_HPP

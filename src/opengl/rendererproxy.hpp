#ifndef RENDERERPROXY_HPP
#define RENDERERPROXY_HPP

#include "openglquickitem.hpp"
#include <QQuickFramebufferObject>

#include <functional>
#include "openglobject.hpp"

class rendererproxy :  public QObject, public QQuickFramebufferObject::Renderer
{
    Q_OBJECT
    typedef std::function<void (std::shared_ptr<QEvent>)> cb;
public:
    explicit rendererproxy(QObject *parent = nullptr);
protected:
    void render() override;
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    void synchronize(QQuickFramebufferObject *item) override;

    OpenGLObject *obj;
private:
    void initCallbacks();
    void make_object(QString tag);

    OpenGLQuickItem *m_quickItem;
    QHash<QEvent::Type,cb> callbacks;

};

#endif // RENDERERPROXY_HPP

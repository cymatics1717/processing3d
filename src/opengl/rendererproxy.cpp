#include "rendererproxy.hpp"

#include <QOpenGLFramebufferObject>
#include <QThread>
#include <QQuickWindow>
#include <iostream>

#include "pclobject.hpp"

rendererproxy::rendererproxy(QObject *parent) : QObject(parent),obj(nullptr),m_quickItem(nullptr)
{

}

void rendererproxy::render()
{
    if(obj == nullptr) return;

    static auto last = std::chrono::high_resolution_clock::now();
    auto current = std::chrono::high_resolution_clock::now();
    auto tmp  = std::chrono::duration_cast<std::chrono::microseconds>(current-last).count();
//    std::cerr <<  1000./tmp<< "kf\n";
    last = current;

    obj->render();
    update();
//    QThread::msleep(10);
}

QOpenGLFramebufferObject *rendererproxy::createFramebufferObject(const QSize &size)
{
    qDebug() << size;
    if(obj){
        obj->reSize(size);
    }
//    QOpenGLFramebufferObjectFormat format;

//    format.setAttachment(QOpenGLFramebufferObject::Depth);
//    format.setSamples(4);
//    format.setTextureTarget(GL_TEXTURE_2D);
//    format.setInternalTextureFormat(GL_RGBA);

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Depth);

    std::unique_ptr<QOpenGLFramebufferObject>framebufferObject(new QOpenGLFramebufferObject(size, format));

    return framebufferObject.release();
}

void rendererproxy::synchronize(QQuickFramebufferObject *item)
{
    if(! m_quickItem ){
        m_quickItem = static_cast<OpenGLQuickItem*>(item);
        make_object(m_quickItem->objectName());
        if(obj){
            obj->initialize();
            initCallbacks();
        }
    }

    if(obj == nullptr) return;

    while (!m_quickItem->events.empty()) {
        auto e = m_quickItem->events.takeFirst();
        e->accept();
//        callbacks.value(e->type(),std::bind(&OpenGLObject::idleEvent, obj,std::placeholders::_1))(e);
    }

    m_quickItem->window()->resetOpenGLState();
}

void rendererproxy::initCallbacks()
{
    callbacks.insert(QEvent::MouseButtonPress,std::bind(&OpenGLObject::mousePressEvent, obj,std::placeholders::_1));
    callbacks.insert(QEvent::MouseButtonRelease,std::bind(&OpenGLObject::mouseReleaseEvent, obj,std::placeholders::_1));
    callbacks.insert(QEvent::MouseButtonDblClick,std::bind(&OpenGLObject::mouseDoubleClickEvent, obj,std::placeholders::_1));
    callbacks.insert(QEvent::MouseMove,std::bind(&OpenGLObject::mouseMoveEvent, obj,std::placeholders::_1));
    callbacks.insert(QEvent::KeyPress,std::bind(&OpenGLObject::keyPressEvent, obj,std::placeholders::_1));
    callbacks.insert(QEvent::KeyRelease,std::bind(&OpenGLObject::keyReleaseEvent, obj,std::placeholders::_1));
    callbacks.insert(QEvent::Wheel,std::bind(&OpenGLObject::wheelEvent, obj,std::placeholders::_1));
    callbacks.insert(QEvent::TouchBegin,std::bind(&OpenGLObject::touchEvent, obj,std::placeholders::_1));
    callbacks.insert(QEvent::TouchUpdate,std::bind(&OpenGLObject::touchEvent, obj,std::placeholders::_1));
    callbacks.insert(QEvent::TouchEnd,std::bind(&OpenGLObject::touchEvent, obj,std::placeholders::_1));
}

void rendererproxy::make_object(QString tag)
{
    if(tag == "pcl")
    {
        auto o = new PCLObject(this);
        connect(m_quickItem,&OpenGLQuickItem::openFile,o,&PCLObject::loadFile);
      //  connect(m_quickItem,&OpenGLQuickItem::openFolder,o,&PanoReader::openlistImageFiles);
      //  connect(m_quickItem,&OpenGLQuickItem::setSwitchType,o,&PanoReader::setSwitchType);
        obj = o;
    }
}

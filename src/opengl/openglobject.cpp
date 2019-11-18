#include "openglobject.hpp"
#include <QtDebug>
OpenGLObject::OpenGLObject(QObject *parent) : QObject(parent)
{

}

OpenGLObject::~OpenGLObject()
{
}

void OpenGLObject::initialize()
{
    initializeOpenGLFunctions();
    glClearColor(0.4f, 0.1f, 0.1f, 1.0f);
}

void OpenGLObject::render()
{
//    qDebug() << "";
}

void OpenGLObject::reSize(const QSize &size)
{
//    qDebug() << size;
}

void OpenGLObject::keyPressEvent(std::shared_ptr<QEvent> event)
{
//    qDebug() << event.get();
}

void OpenGLObject::keyReleaseEvent(std::shared_ptr<QEvent> event)
{
    //    qDebug() << event.get();
}

void OpenGLObject::idleEvent(std::shared_ptr<QEvent> event)
{
     qDebug() << event.get();
}

void OpenGLObject::mousePressEvent(std::shared_ptr<QEvent> event)
{
//    qDebug() << event.get();

}

void OpenGLObject::mouseMoveEvent(std::shared_ptr<QEvent> event)
{
//    qDebug() << event.get();

}

void OpenGLObject::mouseReleaseEvent(std::shared_ptr<QEvent> event)
{
//    qDebug() << event.get();

}

void OpenGLObject::mouseDoubleClickEvent(std::shared_ptr<QEvent> event)
{
//    qDebug() << event.get();

}

void OpenGLObject::wheelEvent(std::shared_ptr<QEvent> event)
{
//    qDebug() << event.get();

}

void OpenGLObject::touchEvent(std::shared_ptr<QEvent> event)
{
//    qDebug() << event.get();

}


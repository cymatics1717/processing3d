#include "openglquickitem.hpp"
#include <QDebug>
#include <QDir>
#include "rendererproxy.hpp"

OpenGLQuickItem::OpenGLQuickItem()
{
//    startTimer(1000);
    installEventFilter(this);
    setMirrorVertically(true);
    setAcceptedMouseButtons(Qt::AllButtons);
//    setAcceptTouchEvents(true);
//    setAcceptHoverEvents(true);
    setFlags(flags()|QQuickItem::ItemAcceptsDrops
             |QQuickItem::ItemIsFocusScope
             |QQuickItem::ItemClipsChildrenToShape
//             |QQuickItem::ItemAcceptsInputMethod
             |QQuickItem::ItemHasContents);

    connect(this,&OpenGLQuickItem::openFolder,this,&OpenGLQuickItem::onOpenFolder);
}

QQuickFramebufferObject::Renderer *OpenGLQuickItem::createRenderer() const
{
    return new rendererproxy();
}

void OpenGLQuickItem::onOpenFolder(const QString &filename)
{
    QString source = QUrl(filename).path();
    if(source.at(0)!=':'&&source.contains(':')){
        source.remove(0,1);
    }

    QDir f(source, "," );
    QStringList filters;
    filters<<"*.png"<<"*.PNG"<<"*.jpg"<<"*.JPG";
    f.setNameFilters(filters);
    f.setFilter(QDir::Files);
    m_images.clear();
    for(auto v:f.entryList()){
        m_images << filename+"/"+v;
    }
    qDebug() << m_images;
    emit imagesChanged();
}

bool OpenGLQuickItem::eventFilter(QObject *o, QEvent *e)
{
    const QEvent::Type t = e->type();
//    qDebug() << "--------------" <<o<< e ;
    if (t == QEvent::MouseButtonPress||t==QEvent::MouseButtonRelease ||t==QEvent::MouseButtonDblClick||t==QEvent::MouseMove) {
        forceActiveFocus();
        handleEvent(std::make_shared<QMouseEvent>(static_cast<const QMouseEvent&>(*e)));
    } else if(t==QEvent::KeyPress||t==QEvent::KeyRelease||t==QEvent::ShortcutOverride){
        auto key_event = std::make_shared<QKeyEvent>(static_cast<const QKeyEvent&>(*e));
        if(key_event->key()==Qt::Key_Return||key_event->key()==Qt::Key_F){
            if(t==QEvent::KeyRelease) toggleWindow();
        } else if(key_event->key()==Qt::Key_Backspace||key_event->key()==Qt::Key_Escape){
            stepback();
        } else if(key_event->key()==Qt::Key_A||key_event->key()==Qt::Key_Z){
            toggleShow();
        } else if(key_event->key()==Qt::Key_Q){
            qApp->quit();
        }
        handleEvent(key_event);
    } else if(t==QEvent::Wheel){
        handleEvent(std::make_shared<QWheelEvent>(static_cast<const QWheelEvent&>(*e)));
    } else if(t==QEvent::HoverMove||t==QEvent::HoverEnter||t==QEvent::HoverLeave){
        handleEvent(std::make_shared<QHoverEvent>(static_cast<const QHoverEvent&>(*e)));
    } else if(t==QEvent::FocusIn||t==QEvent::FocusOut){
        handleEvent(std::make_shared<QFocusEvent>(static_cast<const QFocusEvent&>(*e)));
    } else if(t==QEvent::Timer){
        handleEvent(std::make_shared<QTimerEvent>(static_cast<const QTimerEvent&>(*e)));
    } else if(t == QEvent::TouchBegin ||t == QEvent::TouchUpdate || t == QEvent::TouchEnd){
        forceActiveFocus();
        handleEvent(std::make_shared<QTouchEvent>(static_cast<const QTouchEvent&>(*e)));
    } else if(t == QEvent::ContextMenu){
        handleEvent(std::make_shared<QContextMenuEvent>(static_cast<const QContextMenuEvent&>(*e)));
    } else {
        if(!static_cast<QInputMethodQueryEvent*>(e)){
            qDebug()<<QString(30,'+') <<"unhandled event: "<< o << e;
            handleEvent(std::make_shared<QEvent>(static_cast<const QEvent&>(*e)));
            return QObject::eventFilter(o,e);
        }
    }

    return true;
}

void OpenGLQuickItem::handleEvent(std::shared_ptr<QEvent> event)
{
    event->accept();
    events.append(event);
    update();
}

#include "eventbus.hpp"
#include <QDebug>
#include <QMetaClassInfo>

eventBus::eventBus(QObject *parent) : QObject(parent)
{

}

bool eventBus::eventFilter (QObject *obj, QEvent *e)
{
    QSet<QString> buff={
        "QQuickApplicationWindow",
        "QGraphicsScene"
    };

    if(true || e->type()==QEvent::MetaCall)
        if(!buff.contains(obj->metaObject()->className())){
        qDebug()<<obj<< "---------"<<e->type();
    }
    return QObject::eventFilter(obj,e);
}

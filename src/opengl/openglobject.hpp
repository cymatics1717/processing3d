#ifndef OPENGLMODEL_HPP
#define OPENGLMODEL_HPP

#include <QEvent>
#include <QMouseEvent>
#include <QObject>
#include <QOpenGLExtraFunctions>
#include <memory>

class OpenGLObject : public QObject,protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit OpenGLObject(QObject *parent = nullptr);
    ~OpenGLObject();
signals:

public:
    virtual void initialize();
    virtual void render();
    virtual void reSize(const QSize &size);
    virtual void keyPressEvent(std::shared_ptr<QEvent> event);
    virtual void keyReleaseEvent(std::shared_ptr<QEvent> event);

    virtual void mousePressEvent(std::shared_ptr<QEvent> event);
    virtual void mouseMoveEvent(std::shared_ptr<QEvent> event);
    virtual void mouseReleaseEvent(std::shared_ptr<QEvent> event);
    virtual void mouseDoubleClickEvent(std::shared_ptr<QEvent> event);

    virtual void wheelEvent(std::shared_ptr<QEvent> event);
    virtual void touchEvent(std::shared_ptr<QEvent> event);


//    virtual void focusInEvent(QFocusEvent *);
//    virtual void focusOutEvent(QFocusEvent *);
//    virtual void hoverEnterEvent(QHoverEvent *event);
//    virtual void hoverMoveEvent(QHoverEvent *event);
//    virtual void hoverLeaveEvent(QHoverEvent *event);
////#if QT_CONFIG(draganddrop)
//    virtual void dragEnterEvent(QDragEnterEvent *);
//    virtual void dragMoveEvent(QDragMoveEvent *);
//    virtual void dragLeaveEvent(QDragLeaveEvent *);
//    virtual void dropEvent(QDropEvent *);
//    virtual void inputMethodEvent(QInputMethodEvent *event);
//    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

};

#endif // OPENGLMODEL_HPP

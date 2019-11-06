#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "eventbus.hpp"
#include "opengl/openglquickitem.hpp"
#include "vtk/qvtkfboitem.hpp"
#include "core/master.hpp"
#include "video/videoitem.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //MESA_GL_VERSION_OVERRIDE=3.2 MESA_GLSL_VERSION_OVERRIDE=150 ./processing3d
    qputenv("MESA_GL_VERSION_OVERRIDE",QString("3.3").toUtf8());
//    qputenv("MESA_GLSL_VERSION_OVERRIDE",QString("150").toUtf8());
//    qputenv("MESA_GL_VERSION_OVERRIDE",QString("4.3").toUtf8());
//    qputenv("QT_QPA_PLATFORM", "webgl:port=12345");

    QString QT_MESSAGE_PATTERN = ""
                                 //                                "[%{threadid}-"
                                 "%{time hh:mm:ss.zzz}-"
                                 "%{if-debug}D%{endif}"
                                 "%{if-info}I%{endif}"
                                 "%{if-warning}W%{endif}"
                                 "%{if-critical}E%{endif}"
                                 "%{if-fatal}F%{endif}]"
                                 "%{if-warning} %{backtrace}%{endif}"
                                 "%{if-critical} %{backtrace}%{endif}"
                                 "%{if-fatal} %{backtrace}%{endif} "
                                 "%{if-info}%{function}:%{line}%{endif}"
                                 "%{if-debug}%{function}:%{line}%{endif}"
                                 "---%{message}";

    qSetMessagePattern(QT_MESSAGE_PATTERN);
    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName("...");
    QGuiApplication::setOrganizationDomain("...");


    qmlRegisterType<OpenGLQuickItem>("SceneGraphRendering", 1, 0, "OpenGL");
    qmlRegisterType<QVTKFBOItem>("SceneGraphRendering", 1, 0, "VTKScene");
    qmlRegisterType<videoItem>("SceneGraphRendering", 1, 0, "VideoSource");

    QQmlApplicationEngine engine;
    Master master;
//    eventBus bus;
//    app.installEventFilter(&bus);

    qDebug() << &engine;

    const QUrl url(QStringLiteral("qrc:/src/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("backend",&master);
    engine.load(url);

    return QGuiApplication::exec();
}

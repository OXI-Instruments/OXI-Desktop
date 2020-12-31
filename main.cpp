#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>

#include "oxihardware.h"

#define DEFAULT_DEV_NAME "Deluge"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    OxiHardware *hw = new OxiHardware();

    hw->setDeviceName(DEFAULT_DEV_NAME);

    QTimer *watchdog = new QTimer();
    app.connect(watchdog, &QTimer::timeout, [&hw](){ hw->detectDevice(); });
    watchdog->start(1000);

    QQmlContext *ctx = engine.rootContext();

    ctx->setContextProperty("hw", hw);


    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

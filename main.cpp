#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <dialbuttonslot.h>
#include <answerbuttonslot.h>
#include <rejectbuttonslot.h>
#include <endcallbuttonslot.h>
#include <QQuickPaintedItem>
#include <QScreen>
#include <screenshotimageprovider.h>
#include <handlesnapshotid.h>
#include <QQuickWindow>
#include <QtAndroidExtras/QAndroidIntent>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    SnapshotImageProvider *imageProvider = new SnapshotImageProvider();
    engine.addImageProvider("snapshotimageprovider",imageProvider);

    QQmlComponent component(&engine,QUrl(QStringLiteral("qrc:/main.qml")));
    QObject *viewLoader = component.create();

    QQuickWindow *window = viewLoader->findChild<QQuickWindow*>("windowObject");

    QObject *fetchSnapshotButton = viewLoader->findChild<QObject*>("fetchSnapshotButtonObject");
    HandleSnapshotId handle;
    imageProvider->setSnapshotObject(&handle);
    imageProvider->setWindow(window);
    QObject::connect(fetchSnapshotButton, SIGNAL(sendSnapshotId(QString)), &handle, SLOT(handleId(QString)));

    QObject *dialButton = viewLoader->findChild<QObject*>("dialButtonObject");
    DialButtonSlot dialButtonSlot;
    QObject::connect(dialButton, SIGNAL(dial(QString)), &dialButtonSlot, SLOT(dial(QString)));

    QObject *answerButton = viewLoader->findChild<QObject*>("answerButtonObject");
    AnswerButtonSlot answerButtonSlot;
    QObject::connect(answerButton, SIGNAL(answer()), &answerButtonSlot, SLOT(answer()));

    QObject *rejectButton = viewLoader->findChild<QObject*>("rejectButtonObject");
    RejectButtonSlot rejectButtonSlot;
    QObject::connect(rejectButton, SIGNAL(reject()), &rejectButtonSlot, SLOT(reject()));

    QObject *endCallButton = viewLoader->findChild<QObject*>("endCallButtonObject");
    EndCallButtonSlot endCallButtonSlot;
    QObject::connect(endCallButton, SIGNAL(endCall(qint32)), &endCallButtonSlot, SLOT(endCall(qint32)));

    return app.exec();
}

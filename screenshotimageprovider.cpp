#include "screenshotimageprovider.h"

SnapshotImageProvider::SnapshotImageProvider()
    :QQuickImageProvider (QQuickImageProvider::Image)
{

}

QImage SnapshotImageProvider::requestImage(const QString &id, QSize *size, const QSize& requestedSize){
    if (id == "fetch") {
        qDebug() << "id :"<< snapshotObject->id;
        return screenshot;
    }
#ifdef Q_OS_MACOS && Q_OS_IOS
    int screenNumber = QApplication::desktop()->screenNumber(QCursor::pos());
    QScreen* screen = QApplication::screens().at(screenNumber);
    QRect screenGeometry = screen->geometry();
    QGuiApplication::primaryScreen();
    QPixmap pixmap = screen->grabWindow(0, screenGeometry.x(), screenGeometry.y(), screenGeometry.width(), screenGeometry.height());
    screenshot = pixmap.toImage();
    qDebug() << "From Mac" << screenshot;
#endif
    return screenshot;
}

#ifndef SCREENSHOTIMAGEPROVIDER_H
#define SCREENSHOTIMAGEPROVIDER_H

#include <QObject>
#include <QQuickImageProvider>
#include <QDebug>
#include <QImage>
#include <QTimer>
#include <QPixmap>
#include <QSettings>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QQmlProperty>
#include <handlesnapshotid.h>
#include <QQuickWindow>

class SnapshotImageProvider:public QQuickImageProvider
{
public:
    SnapshotImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize& requestedSize) override;
    void setSnapshotObject(HandleSnapshotId *handle){
        snapshotObject = handle;
    }
    void setWindow(QQuickWindow *window){
        m_window = window;
    }
private:
    QImage screenshot;
    HandleSnapshotId *snapshotObject;
    QQuickWindow *m_window;
};

#endif // SCREENSHOTIMAGEPROVIDER_H

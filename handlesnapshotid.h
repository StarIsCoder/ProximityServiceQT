#ifndef HANDLESNAPSHOTID_H
#define HANDLESNAPSHOTID_H

#include <QObject>
#include <QDebug>
class HandleSnapshotId:public QObject
{
    Q_OBJECT
public:
    HandleSnapshotId();
    QString id;
public slots:
    void handleId(QString s) {
        id = s;
    }

};

#endif // HANDLESNAPSHOTID_H

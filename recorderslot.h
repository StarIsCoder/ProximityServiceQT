#ifndef RECORDERSLOT_H
#define RECORDERSLOT_H

#include <QObject>
#include <QDebug>
#include <recordslotmac.h>

class RecorderSlot:public QObject
{
     Q_OBJECT
public slots:
    void receiveRecordSignal(){
#ifdef Q_OS_MACOS && Q_OS_IOS
        qDebug() << "mac recorder start";
        audioRecorderMac = new AudioRecorderMac();
        audioRecorderMac->startRecord();
#endif
    };
public:
    RecorderSlot();
    AudioRecorderMac *audioRecorderMac;
};

#endif // RECORDERSLOT_H

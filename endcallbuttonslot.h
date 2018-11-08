#ifndef ENDCALLBUTTONSLOT_H
#define ENDCALLBUTTONSLOT_H

#include <QObject>
#include <QDebug>

class EndCallButtonSlot:public QObject
{
    Q_OBJECT
public slots:
    void endCall(qint32 callId){
        endCallByService(callId);
    }
public:
    EndCallButtonSlot();
    void endCallByService(qint32 callId);
};

#endif // ENDCALLBUTTONSLOT_H

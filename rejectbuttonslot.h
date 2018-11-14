#ifndef REJECTBUTTONSLOT_H
#define REJECTBUTTONSLOT_H

#include <QObject>
#include <QDebug>

class RejectButtonSlot:public QObject
{
    Q_OBJECT
public slots:
    void reject(){
        rejectByService();
    }
public:
    RejectButtonSlot();
    void rejectByService();

};

#endif // REJECTBUTTONSLOT_H

#include "dialbuttonslot.h"

DialButtonSlot::DialButtonSlot()
{

}

void DialButtonSlot::dialByService(QString &message){
    qDebug() << "Dialing :" << message;
}

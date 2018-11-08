#include "endcallbuttonslot.h"

EndCallButtonSlot::EndCallButtonSlot()
{

}

void EndCallButtonSlot::endCallByService(qint32 callId){
    qDebug()<< "End call :" << callId;
}

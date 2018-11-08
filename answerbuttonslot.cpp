#include "answerbuttonslot.h"

AnswerButtonSlot::AnswerButtonSlot(QObject *parent):
    QObject(parent)
{

}

void AnswerButtonSlot::answerByService()
{
    qDebug() << "Answer call";
}

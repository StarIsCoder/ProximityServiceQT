#ifndef ANSWERBUTTONSLOT_H
#define ANSWERBUTTONSLOT_H

#include <QObject>
#include <QDebug>

class AnswerButtonSlot:public QObject {
    Q_OBJECT
public slots:
    void answer(){
        answerByService();
    }
public:
    explicit AnswerButtonSlot(QObject *parent = 0);
    void answerByService();
};
#endif // ANSWERBUTTONSLOT_H

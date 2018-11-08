#ifndef DIALBUTTONSLOT_H
#define DIALBUTTONSLOT_H

#include <QObject>
#include <QDebug>

class DialButtonSlot:public QObject {
    Q_OBJECT
public slots:
    void dial(QString message) {
        dialByService(message);
    }
public:
    DialButtonSlot();
    void dialByService(QString &message);
};
#endif // DIALBUTTONSLOT_H

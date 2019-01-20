#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVector>

#include "messagepair.h"

class MessageReader : public QObject
{
    Q_OBJECT
public:
    MessageReader(QByteArray *aMessage);

    static bool isValid(QByteArray &aMsg, QString &rError);

    void parse();
    int getNumberOfPairs() const;
    int hasKey(QString aKey);

    MessagePair* getMessagePairByIndex(int aIdx) const;
signals:

public slots:

private:
    QByteArray *mMessage;
    QVector<MessagePair*> mMessagePairs;
};

#endif // MESSAGEREADER_H

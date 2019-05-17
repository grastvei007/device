#ifndef VICTRONENERGY_H
#define VICTRONENERGY_H

#include <QObject>
#include <QByteArray>
#include <QMap>

#include "../device.h"

#include <tagsystem/tagsocket.h>

class VictronEnergy : public Device
{
public:
    enum State
    {
        eIdle,
        eBegin,
        eName,
        eValue,
        eChecksum
    };
    VictronEnergy();

    int getPid() const override;
    void dataRead(QByteArray aData) override;

private:
    void recordFrame(char c);
    void putValuesOnTags();
    QString pidToDeviceName(const QString &aPid);

    int mChecksum;
    QByteArray mFrame;
    State mState;
    QString mName;
    QString mValue;
    QMap<QString, QString> mReadFrame;
    QMap<QString, TagSocket*> mTagsockets;
};

#endif // VICTRONENERGY_H

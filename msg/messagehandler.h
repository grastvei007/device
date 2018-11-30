#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QMap>

#include <tagsystem/tag.h>

class Device;


class MessageHandler : public QObject
{
    Q_OBJECT
public:
    MessageHandler(Device *aDevice);

private slots:
    void onDeviceData(QByteArray aData);

private:
    void parseData(QByteArray aMsg);
    void extractMessage();

private:
    Device *mDevice;

    QByteArray mDataBuffer;

    QMap<QString, Tag*> mTags;
};

#endif // MESSAGEHANDLER_H

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QMap>

#include <tagsystem/tag.h>

class Message;
class Device;


class MessageHandler : public QObject
{
    Q_OBJECT
public:
    MessageHandler(Device *aDevice);

signals:
    void doubleValue(QString,double); ///< key, value extracted from atmega message.
    void floatValue(QString,float); ///< key, value extracted from atmega message.
    void intValue(QString,int); ///< key, value extracted from atmega message.
    void boolValue(QString,bool); ///< key, value extracted from atmega message.
    void stringValue(QString,QString); ///< key, value extracted from atmega message.

private slots:
    void onDeviceData(QByteArray aData);

private:
    void parseData(QByteArray aMsg);
    void extractMessage();
    void parseAtmegaMessage(const Message &aMessage); ///< emits key, value signals.
private:
    Device *mDevice;
    bool mIsAtmega;

    QByteArray mDataBuffer;

    QMap<QString, Tag*> mTags;
};

#endif // MESSAGEHANDLER_H

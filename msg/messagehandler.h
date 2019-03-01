#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QMap>

#include <tagsystem/tag.h>
#include <tagsystem/tagsocket.h>

class Message;
class Device;

/**
 * @brief The MessageHandler class
 *
 * The MessageHandler recieve and validates messages from a device,
 * the message are translated into key/value pairs and emitted.
 * The handler also translate the values into the tagsystem,
 * and automatic create tags for a key, and write the value to the tag.
 *
 * There are some reserved keyname for communication with an atmega
 * device.
 *      "deviceName" contains the device name, and this name is the base
 *      for the subsystem name on the tagsystem. A device name has to be set
 *      before the communication begins.
 *
 *      "createTags" can be sent to the device to force it to send tagnames.
 *      this is usefull for tags the atmega is reading.
 *
 */
class MessageHandler : public QObject
{
    Q_OBJECT
public:
    MessageHandler(Device *aDevice);

signals:
    void doubleValue(QString,double); ///< key, value extracted from atmega message.
   // void floatValue(QString,float); ///< key, value extracted from atmega message.
    void intValue(QString,int); ///< key, value extracted from atmega message.
    void boolValue(QString,bool); ///< key, value extracted from atmega message.
    void stringValue(QString,QString); ///< key, value extracted from atmega message.

private slots:
    void onDeviceData(QByteArray aData);
    void pollAtmegaDeviceName();

    void onDoubleValue(QString aKey, double aValue);
    void onIntValue(QString aKey, int aValue);
    void onBoolValue(QString aKey, bool aValue);
    void onStringValue(QString aKey, QString aValue);

    //handle changes
    void onTagSocketValueChanged(TagSocket *aTagSocket);
private:
    void parseData(QByteArray aMsg);
    void extractMessage();
    void parseAtmegaMessage(const Message &aMessage); ///< emits key, value signals.
private:
    Device *mDevice;
    bool mIsAtmega;
    bool mDeviceNameIsSet;

    QByteArray mDataBuffer;

    QMap<QString, TagSocket*> mTagSockets;
};

#endif // MESSAGEHANDLER_H

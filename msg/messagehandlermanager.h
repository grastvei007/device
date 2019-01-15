#ifndef MESSAGEHANDLERMANAGER_H
#define MESSAGEHANDLERMANAGER_H

#include <QObject>
#include <QMap>

#include "../device.h"
#include "messagehandler.h"


/**
 * @brief The MessageHandlerManager class
 *
 * Automatic crete tags and update values from a serial device.
 * Message that are handled is sent with the following protocol.
 *
 *  Every message start with <msg and end with >
 *  after the start follow a int value of 4 bytes in plain text, it describes
 *  the size of the message include start and end markers.
 *  just before the end '>' is a control byte [0-255] used as checksum.
 *  This is calculated with the sum of all bytes (char value) in the message exept
 *  the checksum, and then module 255 gives the checksum if there are no errors.
 *
 *  Sum all bytes modulo 255 = checksum
 *
 */
class MessageHandlerManager : public QObject
{
    Q_OBJECT
public:
    static MessageHandlerManager& sGetInstance();

    void createMessageHandlerForDevice(Device *aDevice);

private slots:
    void onDeviceDestroyed(Device *aDevice);

private:
    MessageHandlerManager();

private:
    QMap<Device*, MessageHandler*> mMessageHandlers;
};

#endif // MESSAGEHANDLERMANAGER_H

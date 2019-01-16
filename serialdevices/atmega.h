#ifndef ATMEGA_H
#define ATMEGA_H

#include "../device.h"

class Message;

class Atmega : public Device
{
    Q_OBJECT
public:
    Atmega();
    int getPid() const {return 42;}
    void dataRead(QByteArray aData);

    bool isDeviceNameSet() const;
    bool sendMessage(const Message &aMessage); ///< send a valid message to device.

    void setDeviceName(const Message &aMessage); ///< set the device name if the massage contains device name.
    void requestDeviceName();
signals:

public slots:
};

#endif // ATMEGA_H

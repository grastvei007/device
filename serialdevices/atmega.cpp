#include "atmega.h"

#include <QByteArray>
#include <QDebug>

#include "../msg/message.h"

Atmega::Atmega() : Device()
{

}


void Atmega::dataRead(QByteArray aData)
{

}


bool Atmega::isDeviceNameSet() const
{
    return getDeviceName().isEmpty();
}

bool Atmega::sendMessage(const Message &aMessage)
{
    if(!aMessage.isValid())
        return false;

    QByteArray message;
    message.append(aMessage.getMessage());
    write(message);
    return true;
}


void Atmega::setDeviceName(const Message &aMessage)
{
    QByteArray str = aMessage.getMessage();
    qDebug() << __FUNCTION__ << str;
    if(!str.contains("deviceName"))
        return;

    QByteArray last = str.split(':').last();
    QByteArray len = last.mid(2,2);
    short size = len.toShort();
    if(last.size() < size+2)
        return;
    QString name = last.mid(4, size);
    qDebug() << __FUNCTION__ << name;
    Device::setDeviceName(name);
}


void Atmega::requestDeviceName()
{
    Message msg;
    bool b = true;
    msg.add(QString("deviceName"), QString(""));
    msg.finnish();
    sendMessage(msg);
}


void Atmega::createTags()
{
    Message msg;
    msg.add(QString("createTags"), QString(""));
    msg.finnish();
    sendMessage(msg);
}

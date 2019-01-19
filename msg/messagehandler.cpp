#include "messagehandler.h"

#include <QStringList>
#include <QDataStream>
#include <QDebug>
#include <QTimer>

#include "device.h"
#include <msg/message.h>
#include <tagsystem/taglist.h>

#include "../serialdevices/atmega.h"

MessageHandler::MessageHandler(Device *aDevice) :
    mDevice(aDevice),
    mIsAtmega(false)
{
    connect(mDevice, &Device::dataRecieved, this, &MessageHandler::onDeviceData);
    mDevice->setOverideDataRead(true);
    if(dynamic_cast<Atmega*>(aDevice))
    {
            mIsAtmega = true;
            QTimer::singleShot(2000, [=](){
                dynamic_cast<Atmega*>(aDevice)->requestDeviceName();
            });
    }
}


void MessageHandler::onDeviceData(QByteArray aData)
{
   // qDebug() << __FUNCTION__ <<   aData;
    mDataBuffer.append(aData);
    extractMessage();
}



void MessageHandler::parseData(QByteArray aMsg)
{
    qDebug() << "Got message: " << aMsg;
    Message msg(aMsg);
    if(msg.isValid() != 1)
    {
        qDebug() << "Invalid message(" << msg.isValid() << ")!!!" ;

        if(mIsAtmega && !dynamic_cast<Atmega*>(mDevice)->isDeviceNameSet())
        {
            QTimer::singleShot(2000, [=](){
                dynamic_cast<Atmega*>(mDevice)->requestDeviceName();
            });
        }
        return;
    }


    if(mIsAtmega)
        parseAtmegaMessage(msg);
}

void MessageHandler::parseAtmegaMessage(const Message &aMessage)
{
    qDebug() << __FUNCTION__;

    QByteArray ba  = aMessage.getMessage();
    ba.remove(0,8);
    // start from first key
    QString key;
    bool hasKey = false;
    QDataStream stream(ba);
    while(!stream.atEnd())
    {
        uint8_t c;
        stream >> c;
        if(char(c) != ':' && !hasKey)
            key.append(c);
        else if(c == ':')
        {
            hasKey = true;
            // skip, got key
        }
        else if(c == 'f')
        {
            float f;
            stream >> f;
            emit floatValue(key, f);
            key.clear();
            hasKey = false;
        }
        else if(c == 'i')
        {
            int i;
            stream >> i;
            emit intValue(key, i);
            key.clear();
            hasKey = false;
        }
        else if(c == 'b')
        {
            qint8 c;
            stream >> c;
            bool b = c == '1' ? true : false;
            emit boolValue(key, b);
            key.clear();
            hasKey = false;
        }
        else if(c == 'c')
        {
            uint8_t len;
            stream >> len;
            QString val;
            for(int i = 0; i<len; ++i)
            {
                qint8 c;
                stream >> c;
                val.append(c);
            }
            emit stringValue(key, val);
            key.clear();
            hasKey = false;
        }

    }

}

void MessageHandler::extractMessage()
{
    int idx = mDataBuffer.indexOf("<msg");
    if(idx < 0)
        return;

    if(mDataBuffer.size() < idx+8)
        return;

    QString msgSize(mDataBuffer.mid(idx+4, 4));
    int size = msgSize.toInt();

    if(mDataBuffer.size() < idx+size)
        return;

    // buffer contains a message.
    QByteArray message = mDataBuffer.mid(idx, size);
    mDataBuffer.remove(0, idx+size);

    parseData(message);
}

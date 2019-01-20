#include "messagehandler.h"

#include <QStringList>
#include <QDataStream>
#include <QDebug>
#include <QTimer>

#include "device.h"
#include <msg/message.h>
#include <tagsystem/taglist.h>

#include "../serialdevices/atmega.h"
#include "messagereader.h"
#include "messagepair.h"



MessageHandler::MessageHandler(Device *aDevice) :
    mDevice(aDevice),
    mIsAtmega(false),
    mDeviceNameIsSet(false)
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

    connect(this, &MessageHandler::boolValue, this, &MessageHandler::onBoolValue);
    connect(this, &MessageHandler::doubleValue, this, &MessageHandler::onDoubleValue);
    connect(this, &MessageHandler::intValue, this, &MessageHandler::onIntValue);
    connect(this, &MessageHandler::stringValue, this, &MessageHandler::onStringValue);
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
    QString error;
    if(!MessageReader::isValid(aMsg, error))
    {
        qDebug() << "Invalid message(" << error << ")!!!" ;

        if(mIsAtmega && !dynamic_cast<Atmega*>(mDevice)->isDeviceNameSet())
        {
            QTimer::singleShot(4000, [=](){
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
    QString error;
    if(!MessageReader::isValid(ba, error))
        qDebug() << error;

    MessageReader reader(&ba);
    reader.parse();

    for(int i=0; i<reader.getNumberOfPairs(); ++i)
    {
        MessagePair *pair = reader.getMessagePairByIndex(i);
        QString key = pair->getKey();
        switch (pair->getType()) {
        case MessagePair::eBool:

            break;
        case MessagePair::eFloat:
        {
            double val = pair->getFloatValue();
            emit doubleValue(key, val);
            break;
        }
        case MessagePair::eInt:
        {
            int val = pair->getIntValue();
            emit intValue(key, val);
            break;
        }
        case MessagePair::eString:
        {
            QString val = pair->getStringValue();
            emit stringValue(key, val);
            break;
        }
        default:
            Q_UNREACHABLE();
            break;
        }
    }

}

void MessageHandler::extractMessage()
{
    int idx = mDataBuffer.indexOf("<msg");
    if(idx < 0)
    {
        qDebug() << mDataBuffer;
        return;
    }

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


void MessageHandler::onDoubleValue(QString aKey, double aValue)
{
    if(!mDeviceNameIsSet)
        return;
}


void MessageHandler::onIntValue(QString aKey, int aValue)
{
    if(!mDeviceNameIsSet)
        return;
}


void MessageHandler::onBoolValue(QString aKey, bool aValue)
{
    if(!mDeviceNameIsSet)
        return;
}

/**
 * @brief MessageHandler::onStringValue
 * @param aKey
 * @param aKey
 */
void MessageHandler::onStringValue(QString aKey, QString aValue)
{
    if(aKey == "deviceName" && !mDeviceNameIsSet)
    {
        mDevice->setDeviceName(aValue);
        mDeviceNameIsSet = true;

        Tag *tag = TagList::sGetInstance().createTag("device", "name", Tag::eString);
        tag->setValue(aValue);

    }
    if(!mDeviceNameIsSet)
        return;


}

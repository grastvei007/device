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
                pollAtmegaDeviceName();
            });
    }

    connect(this, &MessageHandler::boolValue, this, &MessageHandler::onBoolValue);
    connect(this, &MessageHandler::doubleValue, this, &MessageHandler::onDoubleValue);
    connect(this, &MessageHandler::intValue, this, &MessageHandler::onIntValue);
    connect(this, &MessageHandler::stringValue, this, &MessageHandler::onStringValue);
}


void MessageHandler::onDeviceData(QByteArray aData)
{
    qDebug() << __FUNCTION__ <<   aData;
    mDataBuffer.append(aData);
    extractMessage();
}


void MessageHandler::pollAtmegaDeviceName()
{
     if(mDeviceNameIsSet)
     {

         return;
     }
     dynamic_cast<Atmega*>(mDevice)->requestDeviceName();
     QTimer::singleShot(4000, [=](){
        pollAtmegaDeviceName();
        });
}


void MessageHandler::parseData(QByteArray aMsg)
{
    qDebug() << "Got message: " << aMsg;
    Message msg(aMsg);
    QString error;
    if(!MessageReader::isValid(aMsg, error))
    {
        qDebug() << "Invalid message(" << error << ")!!!" ;

       /* if(mIsAtmega && !dynamic_cast<Atmega*>(mDevice)->isDeviceNameSet())
        {
            QTimer::singleShot(4000, [=](){
                dynamic_cast<Atmega*>(mDevice)->requestDeviceName();
            });
        }*/
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
        {
            bool val = pair->getBoolValue();
            emit boolValue(key, val);
            break;
        }
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

    if(mTagSockets.contains(aKey))
        mTagSockets[aKey]->writeValue(aValue);
    else
    {
        Tag *tag = TagList::sGetInstance().createTag(mDevice->getDeviceName(), aKey, Tag::eDouble);
        tag->setValue(aValue);
        TagSocket *socket = TagSocket::createTagSocket(mDevice->getDeviceName(), aKey, TagSocket::eDouble);
        socket->hookupTag(tag);
        connect(socket, qOverload<TagSocket*>(&TagSocket::valueChanged), this, &MessageHandler::onTagSocketValueChanged);
        mTagSockets[aKey] = socket;
    }
}


void MessageHandler::onIntValue(QString aKey, int aValue)
{
    if(!mDeviceNameIsSet)
        return;

    if(mTagSockets.contains(aKey))
        mTagSockets[aKey]->writeValue(aValue);
    else
    {
        Tag *tag = TagList::sGetInstance().createTag(mDevice->getDeviceName(), aKey, Tag::eInt);
        tag->setValue(aValue);
        TagSocket *socket = TagSocket::createTagSocket(mDevice->getDeviceName(), aKey, TagSocket::eInt);
        socket->hookupTag(tag);
        connect(socket, qOverload<TagSocket*>(&TagSocket::valueChanged), this, &MessageHandler::onTagSocketValueChanged);
        mTagSockets[aKey] = socket;
    }
}


void MessageHandler::onBoolValue(QString aKey, bool aValue)
{
    if(!mDeviceNameIsSet)
        return;

    if(mTagSockets.contains(aKey))
    {
        mTagSockets[aKey]->writeValue(aValue);
    }
    else
    {
        Tag *tag = TagList::sGetInstance().createTag(mDevice->getDeviceName(), aKey, Tag::eBool);
        tag->setValue(aValue);
        TagSocket *socket = TagSocket::createTagSocket(mDevice->getDeviceName(), aKey, TagSocket::eBool);
        socket->hookupTag(tag);
        connect(socket, qOverload<TagSocket*>(&TagSocket::valueChanged), this, &MessageHandler::onTagSocketValueChanged);
        mTagSockets[aKey] = socket;
    }

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
        TagSocket *socket = TagSocket::createTagSocket("device", "name", TagSocket::eString);
        socket->hookupTag(tag);
        //connect(socket, qOverload<TagSocket*(&TagSocket::valueChanged), this, &MessageHandler::onTagSocketValueChanged);
        mTagSockets["name"] = socket;
        if(mIsAtmega)
            dynamic_cast<Atmega*>(mDevice)->createTags();

        return;

    }
    if(!mDeviceNameIsSet)
        return;

    if(mTagSockets.contains(aKey))
        mTagSockets[aKey]->writeValue(aValue);
    else
    {
        Tag *tag = TagList::sGetInstance().createTag(mDevice->getDeviceName(), aKey, Tag::eString);
        tag->setValue(aValue);
        TagSocket *socket = TagSocket::createTagSocket(mDevice->getDeviceName(), aKey, TagSocket::eString);
        socket->hookupTag(tag);
        mTagSockets[aKey] = socket;
    }
}

/**
 * @brief MessageHandler::onTagSocketValueChanged
 *
 * Listen to changes on tagsocket that are created from messages from
 * the device, and send values back to the device.
 *
 * @param aTagSocket
 */
void MessageHandler::onTagSocketValueChanged(TagSocket *aTagSocket)
{
    QString key = aTagSocket->getName();
    switch (aTagSocket->getType()) {
    case TagSocket::eBool:
    {
        bool value;
        if(aTagSocket->readValue(value))
        {
            Message msg;
            msg.add(key, value);
            msg.finnish();
            mDevice->write(msg.getMessage());
        }
        break;
    }
    case TagSocket::eDouble:
    {
        double value;
        if(aTagSocket->readValue(value))
        {
            Message msg;
            msg.add(key, value);
            msg.finnish();
            mDevice->write(msg.getMessage());
        }
        break;
    }
    case TagSocket::eInt:
    {
        int value;
        if(aTagSocket->readValue(value))
        {
            Message msg;
            msg.add(key, value);
            msg.finnish();
            mDevice->write(msg.getMessage());
        }
        break;
    }
    case TagSocket::eString:
    {
        QString value;
        if(aTagSocket->readValue(value))
        {
            Message msg;
            msg.add(key,value);
            msg.finnish();
            mDevice->write(msg.getMessage());
        }
        break;
    }
    default:
        Q_UNREACHABLE();
    }
}

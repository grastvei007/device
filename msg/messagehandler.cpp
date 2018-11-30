#include "messagehandler.h"

#include <QStringList>

#include "device.h"
#include <msg/message.h>
#include <tagsystem/taglist.h>


MessageHandler::MessageHandler(Device *aDevice) :
    mDevice(aDevice)
{
    connect(mDevice, &Device::dataRecieved, this, &MessageHandler::onDeviceData);
    mDevice->setOverideDataRead(true);
}


void MessageHandler::onDeviceData(QByteArray aData)
{

    mDataBuffer.append(aData);
    extractMessage();
}



void MessageHandler::parseData(QByteArray aMsg)
{
    Message msg((unsigned char*)aMsg.data());
    if(msg.isValid() != 1)
        return;

    aMsg.remove(0,8); // remove header.
    aMsg.remove(aMsg.size()-2, 2); // remove last two bytes, checksum + end

    QString str(aMsg);
    QStringList tupples = str.split(",");
    for(int i=0; i<tupples.size(); ++i)
    {
        QString key = tupples.at(i).split(":").first();
        QString val = tupples[i].remove(key);

        QString tagName = QString("%1.%2").arg(mDevice->getDeviceName()).arg(key);

        char type = val.at(1).toLatin1();
        if(type == 'f') // float
        {
            if(!mTags.contains(tagName))
            {
                Tag *tag = TagList::sGetInstance().createTag(mDevice->getDeviceName(), key, Tag::eDouble);
                mTags[tagName] = tag;
            }
            val.remove(0,2);
            if(val.size() != 4)
                return;
            float f;
            f = *reinterpret_cast<const float*>(val.data());
            mTags[key]->setValue((double)f);

        }
        else if(type == 'i') // int
        {
            if(!mTags.contains(key))
            {
                Tag *tag = TagList::sGetInstance().createTag(mDevice->getDeviceName(), key, Tag::eInt);
                mTags[tagName] = tag;
            }

            val.remove(0,2);
            if(val.size() != 4)
                return;
            int i;
            i = *reinterpret_cast<const int*>(val.data());
            mTags[key]->setValue(i);
        }
        else if(type == 'd') // double
        {
            if(!mTags.contains(key))
            {
                Tag *tag = TagList::sGetInstance().createTag(mDevice->getDeviceName(), key, Tag::eDouble);
                mTags[tagName] = tag;
            }
            val.remove(0,2);
            if(val.size() != 4)
                return;
            double d;
            d = *reinterpret_cast<const double*>(val.data());
            mTags[key]->setValue(d);
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

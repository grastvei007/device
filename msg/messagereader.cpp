#include "messagereader.h"
#include <QDebug>

MessageReader::MessageReader(QByteArray *aMessage) :
    mMessage(aMessage)
{

}

bool MessageReader::isValid(QByteArray &message, QString &rError)
{
    if (!message.startsWith("<msg")) {
        rError.append("Message does not start with, <msg\n");
        return false;
    }
    if (!message.endsWith(">")) {
        rError.append("Message does not end with, >\n");
        return false;
    }
    if (message.size() < 10) {
        rError.append("Message size is to small\n");
        return false;
    }

    QString size(message.mid(4, 2));
    int n = size.toInt();
    if (message.size() != n + 1) {
        rError.append("Message size is wrong\n");
        return false;
    }

    int sum = 0;
    for (int i = 0; i < message.size(); ++i) {
        if (i == message.size() - 2)
            continue;
        sum += (uint8_t) message[i];
    }

    int r = (uint8_t) message[message.size() - 2];

    if (sum % 256 != r) {
        rError.append(QString("Message checksum is wrong sum, %1, r=%2, ").arg(sum).arg(sum % 256));
        return false;
    }

    return true;
}


void MessageReader::parse()
{
    QString key;
    bool hasKey = false;
    for(int i=8; i<mMessage->size(); ++i)
    {
        if(mMessage->at(i) == ':')
        {
            hasKey = true;
            continue;
        }

        if(!hasKey)
            key.append(mMessage->at(i));
        else
        {
            if(mMessage->at(i) == 'c')
            {
                union U
                {
                    short s;
                    char byte[2];
                }u;
                u.byte[0] = mMessage->at(++i);
                u.byte[1] = mMessage->at(++i);
                QByteArray val = mMessage->mid(++i, u.s);
                i += u.s;
                MessagePair *pair = new MessagePair;
                pair->setKey(key);
                pair->setStringFromBytess(val.data(), u.s);
                mMessagePairs.push_back(pair);
                hasKey = false;
                key.clear();
            }
            else if(mMessage->at(i) == 'f')
            {
                char bytes[4];
                bytes[0] = mMessage->at(++i);
                bytes[1] = mMessage->at(++i);
                bytes[2] = mMessage->at(++i);
                bytes[3] = mMessage->at(++i);
                MessagePair *pair = new MessagePair;
                pair->setKey(key);
                pair->setFloatFromBytes(bytes);
                mMessagePairs.push_back(pair);

                hasKey = false;
                key.clear();
            }
            else if(mMessage->at(i) == 'i')
            {
                char bytes[4];
                bytes[0] = mMessage->at(++i);
                bytes[1] = mMessage->at(++i);
                bytes[2] = mMessage->at(++i);
                bytes[3] = mMessage->at(++i);
                MessagePair *pair = new MessagePair;
                pair->setKey(key);
                pair->setIntFromBytes(bytes);
                mMessagePairs.push_back(pair);

                hasKey = false;
                key.clear();
            }
            else if(mMessage->at(i) == 'b')
            {
                MessagePair *pair = new MessagePair;
                pair->setKey(key);
                pair->setBoolFromByte(mMessage->at(++i));
                mMessagePairs.push_back(pair);

                hasKey = false;
                key.clear();
            }
        }
    }
}


int MessageReader::getNumberOfPairs() const
{
    return mMessagePairs.size();
}


int MessageReader::hasKey(QString aKey)
{
    int r = -1;

    for(int i=0; i<mMessagePairs.size(); ++i)
    {
        if(mMessagePairs.at(i)->getKey() == aKey)
            return i;
    }

    return r;
}


MessagePair* MessageReader::getMessagePairByIndex(int aIdx) const
{
    if(aIdx < 0 || aIdx > mMessagePairs.size())
        return nullptr;
    return mMessagePairs.at(aIdx);
}

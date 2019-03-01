#include "messagereader.h"
#include <QDebug>

MessageReader::MessageReader(QByteArray *aMessage) :
    mMessage(aMessage)
{

}

/**
 * @brief MessageReader::isValid
 *
 * Validated an array containing a message.
 *
 * @param aMsg
 * @return true/false.
 */
bool MessageReader::isValid(QByteArray &aMsg, QString &rError)
{
    if(!aMsg.startsWith("<msg"))
    {
        rError.append("Message does not start with, <msg\n");
        return false;
    }
    if(!aMsg.endsWith(">"))
    {
        rError.append("Message does not end with, >\n");
        return false;
    }
    if(aMsg.size() < 10)
    {
        rError.append("Message size is to small\n");
        return false;
    }


    QString size(aMsg.mid(4,4));
    int n = size.toInt();
    if( aMsg.size() != n)
    {
        rError.append("Message size is wrong\n");
        return false;
    }
    int sum = 0;
    for(int i=0; i<n; ++i)
    {
        if(i == n-2)
            continue;

        int c = (int)aMsg.at(i);
        qDebug() << "c(" << c << ") " << (char)c;
        // just make sure it is in range.
        while(c < 0)
            c += 256;
        while(c > 256)
            c -= 256;

        sum += c;
    }

    int r = sum % 256;
    int t = (int)aMsg.at(n-2);
    while(t < 0)
        t += 256;
    if(t != r)
    {
        rError.append(QString("Message checksum is wrong sum, %1, r=%2, cs=%3").arg(sum).arg(r).arg(t));
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

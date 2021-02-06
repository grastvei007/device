#include "message.h"

#include <QDebug>

#include <tagsystem/tagsocket.h>


Message::Message() :
    mIsValid(true)
{
    mMessage = "<msg0000";
}


Message::Message(const QByteArray &aMsg) : mMessage(aMsg)
{
   // mIsValid = validateMessage(aMsg);
}


Message::Message(const TagSocket &aTagSocket)
{
    mMessage = "<msg0000";
    QString key = aTagSocket.getName();
   /* switch (aTagSocket.getType()) {
    case TagSocket::eDouble:
    {
        double val;
        aTagSocket.readValue(val);
        add(key, val);
        break;
    }
    case TagSocket::eInt:
    {
        int val;
        aTagSocket.readValue(val);
        add(key, val);
        break;
    }
    case TagSocket::eBool:
    {
        bool val;
        aTagSocket.readValue(val);
        add(key, val);
        break;
    }
    case TagSocket::eString:
    {
        QString val;
        aTagSocket.readValue(val);
        add(key, val);
        break;
    }
    default:
        Q_UNREACHABLE();
        break;
    }
    finnish();*/
}


int Message::isValid() const
{
    return mIsValid;
}

void Message::add(const QString &aKey, double aValue)
{

}


void Message::add(const QString &aKey, float aValue)
{
    QString pair(aKey);
    pair.append(":f");
    union U
    {
        float f;
        char bytes[4];
    }u;
    u.f = aValue;
    mMessage.append(pair);
    mMessage.append(u.bytes[0]);
    mMessage.append(u.bytes[1]);
    mMessage.append(u.bytes[2]);
    mMessage.append(u.bytes[3]);
}


void Message::add(const QString &aKey, int aValue)
{
    QString pair(aKey);
    pair.append(":i");

    union U
    {
        int i;
        char bytes[4];
    }u;
    u.i = aValue;
    mMessage.append(pair);
    mMessage.append(u.bytes[0]);
    mMessage.append(u.bytes[1]);
    mMessage.append(u.bytes[2]);
    mMessage.append(u.bytes[3]);
}

void Message::add(const QString &aKey, bool aValue)
{
    QString pair(aKey);
    pair.append(":b");
    if(aValue)
        pair += char(1);
    else
        pair += char(0);
    mMessage.append(pair);
}


void Message::add(const QString &aKey, QString aValue)
{
    QString pair(aKey);
    unsigned char buffer[2];
    short s = (short)aValue.size();
    memcpy(buffer, (unsigned char*)&s, 2);
    pair.append(QString(":c%1%2").arg(buffer[0]).arg(buffer[1]));
    pair.append(aValue);
    mMessage.append(pair);
}


void Message::finnish()
{
    mMessage.append("0>");
    QByteArray size = QString::number(mMessage.size()).toLatin1();

    int t = size.size();
    for(int i=t; i<4; ++i)
        size.prepend("0");
    qDebug() << "Message size: " << size << "t " << t;
    if(t > 4)
        t = 4; // some error, this message will fail.
    mMessage.replace(4, size.size(), size);

    calcCheckcode();
}


QByteArray Message::getMessage() const
{
    return mMessage;
}


QString Message::getAtmegaDeviceName()
{

}

void Message::calcCheckcode()
{
    int n = 0;
    for(int i=0; i<mMessage.size(); ++i)
    {
        if(i != mMessage.size() -2)
            n += (int)mMessage.at(i);
    }

    mMessage[mMessage.size()-2] = (char)(n % 255);
}


int Message::validateMessage(const QByteArray &aMsg)
{
    if(aMsg.size() < 8)
        return -1;

    if(!aMsg.startsWith("<msg"))
    {
        return -2;
    }

    QString temp = aMsg.mid(4, 4);
    int messageSize = temp.toInt();

    int n = 0;
    for(int i=0; i<messageSize; ++i)
    {
        if(i != messageSize-2)
            n += (int)aMsg.at(i);
    }

    int r = n % 256;
    int c = (int)aMsg[messageSize-2];
    if(c < 0)
        c += 256;
    if( r != c)
        return -3;

    mMessage = aMsg;
    return 1;
}

#include "messagepair.h"
#include <QByteArray>

MessagePair::MessagePair()
{

}

void MessagePair::setKey(QString aKeyName)
{
    mKey = aKeyName;
}


void MessagePair::setType(MessagePair::Type aType)
{
    mType = aType;
}


void MessagePair::setFloatFromBytes(char *aValue)
{
    union U
    {
        float f;
        char bytes[4];
    }u;
    u.bytes[0] = aValue[0];
    u.bytes[1] = aValue[1];
    u.bytes[2] = aValue[2];
    u.bytes[3] = aValue[3];

    mDoubleValue = (double)u.f;
    mType = eFloat;

}


void MessagePair::setStringFromBytess(char *aValue, int aSize)
{
    QByteArray ba(aValue, aSize);
    mStringValue = ba;
    mType = eString;
}


void MessagePair::setBoolFromByte(char c)
{
    mBoolValue = ( (int)c == 1) ? true : false;
    mType = eBool;
}


void MessagePair::setIntFromBytes(char *aValue)
{
    union U
    {
        int i;
        char bytes[4];
    }u;
    u.bytes[0] = aValue[0];
    u.bytes[1] = aValue[1];
    u.bytes[2] = aValue[2];
    u.bytes[3] = aValue[3];

    mIntValue = u.i;
    mType = eInt;
}


QString MessagePair::getKey() const
{
    return mKey;
}


MessagePair::Type MessagePair::getType() const
{
    return mType;
}


double MessagePair::getFloatValue() const
{
    return mDoubleValue;
}


QString MessagePair::getStringValue() const
{
    return mStringValue;
}


bool MessagePair::getBoolValue() const
{
    return mBoolValue;
}


int MessagePair::getIntValue() const
{
    return mIntValue;
}

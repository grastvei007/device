#ifndef MESSAGEPAIR_H
#define MESSAGEPAIR_H

#include <QObject>

class MessagePair
{
public:
    enum Type
    {
        eFloat = 0,
        eString,
        eBool,
        eInt
    };
    MessagePair();

    void setKey(QString aKeyName);
    void setType(Type aType);

    void setFloatFromBytes(char *aValue); ///< 4 bytes
    void setStringFromBytess(char *aValue, int aSize);
    void setBoolFromByte(char c); ///< val 0 of 1
    void setIntFromBytes(char *aValue); ///< 4 bytes


    QString getKey() const;
    Type getType() const;
    double getFloatValue() const;
    QString getStringValue() const;
    bool getBoolValue() const;
    int getIntValue() const;

private:
    QString mKey;
    Type mType;

    double mDoubleValue;
    int mIntValue;
    QString mStringValue;
    bool mBoolValue;

};

#endif // MESSAGEPAIR_H

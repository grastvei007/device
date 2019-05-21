#include "victronenergy.h"

#include <QDebug>

#include <tagsystem/taglist.h>

VictronEnergy::VictronEnergy() :
    mChecksum(0),
    mState(eIdle),
    mProductName()
{

}


int VictronEnergy::getPid() const
{
    return 12;
}


void VictronEnergy::dataRead(QByteArray aData)
{
    //qDebug() << aData;

    for(int i=0; i<aData.size(); ++i)
        recordFrame(aData[i]);
}


void VictronEnergy::recordFrame(char c)
{
    mChecksum += c;

    c = toupper(c);

    switch (mState)
    {
    case eIdle:
        {
        switch (c)
            {
            case '\n':
                mName.clear();
                mValue.clear();
                mReadFrame.clear();
                mState = eName;
                break;
            case '\r':
            default:
                break;

            }
        break;
    }
    case eBegin:
        break;
    case eName:
    {
        switch(c)
        {
            case '\t':
                if(mName.contains("Checksum", Qt::CaseInsensitive))
                {
                    mState = eChecksum;
                    break;
                }

                mState = eValue;
                break;

        default:
            mName.append(c);
            break;
        }

        break;
    }
    case eValue:
    {
        switch (c)
        {
            case '\n':
                mReadFrame[mName] = mValue;
                mState = eName;
                mName.clear();
                mValue.clear();
                break;

            case 'r':
                break;
        default:
            mValue.append(c);
            break;
        }
        break;
    }
    case eChecksum:

        int s = mChecksum % 256;
        if(s != 0)
            qDebug() << "Invalid frame, check: " << s;
        else
        {
            qDebug() << "Device name: " << mProductName;
            if(mReadFrame.contains("PID") && mProductName.isEmpty())
            {
                QString pid = mReadFrame["PID"];
                mProductName = pidToDeviceName(pid);
            }
            else
                putValuesOnTags();
        }
        mChecksum = 0;
        mState = eIdle;
        break;

    }
}


void VictronEnergy::putValuesOnTags()
{

    for(auto iter = mReadFrame.begin(); iter != mReadFrame.end(); ++iter)
    {
        if(iter.key() == "PID")
            continue;
        if(mTagsockets.contains(iter.key()))
        {
            TagSocket *tagsocket = mTagsockets[iter.key()];
            switch (tagsocket->getType())
            {
                case TagSocket::eInt:
                {
                    int value = iter.value().toInt();
                    tagsocket->writeValue(value);
                    break;
                }
            case TagSocket::eDouble:
            {
                double value = iter.value().toDouble() / 1000.0;
                tagsocket->writeValue(value);
                break;
            }
            case TagSocket::eBool:
            case TagSocket::eString:
                break;
            }
        }
        else
        {
          createTagSocket(iter.key(), iter.value());
        }
    }
}

QString VictronEnergy::pidToDeviceName(const QString &aPid)
{
    if(aPid.contains("0XA381"))
        qDebug() << "BMV712_Smart";
    return QString("BMV712_Smart");
}

void VictronEnergy::createTagSocket(const QString &aName, const QString &aValue)
{
  switch (stringToValue(aName))
    {
        case eVoltage:
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble);
            double value = aValue.toInt() /1000.0;
            tag->setValue(value);
            TagSocket *tagSocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            tagSocket->hookupTag(tag);
            mTagsockets[aName] = tagSocket;
            break;
        }
        case eAmphere:
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble);
            double value = aValue.toInt() / 1000.0;
            tag->setValue(value);
            TagSocket *tagSocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            tagSocket->hookupTag(tag);
            mTagsockets[aName] = tagSocket;
            break;
        }
        case ePower:
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eInt);
            int value = aValue.toInt();
            tag->setValue(value);
            TagSocket *tagSocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eInt);
            tagSocket->hookupTag(tag);
            mTagsockets[aName] = tagSocket;
            break;
        }
        case eSoc: // SOC
        {
            Tag* tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble);
            double val = aValue.toInt() / 10.0;
            tag->setValue(val);
            TagSocket* tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            tagsocket->hookupTag(tag);
            mTagsockets[aName] = tagsocket;
            break;
        }
        case eTimeToGo: //TTG
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eInt);
            int val = aValue.toInt();
            tag->setValue(val);
            TagSocket *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            tagsocket->hookupTag(tag);
            mTagsockets[aName] = tagsocket;
            break;
        }
        case eConsumedAmpHours: // CE, mAh
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble);
            double val = aValue.toInt() / 1000.0;
            tag->setValue(val);
            TagSocket *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            tagsocket->hookupTag(tag);
            mTagsockets[aName] = tagsocket;
            break;
        }
        case eDepthDepestDischarge: // H1, mAh
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble);
            double val = aValue.toInt() / 1000.0;
            tag->setValue(val);
            TagSocket *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            tagsocket->hookupTag(tag);
            mTagsockets[aName] = tagsocket;
            break;
        }
        case eDepthLastDischarge: // H2 mAh
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble);
            double val = aValue.toInt() / 1000.0;
            tag->setValue(val);
            TagSocket *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            tagsocket->hookupTag(tag);
            mTagsockets[aName] = tagsocket;
            break;
        }
        case eCumulativeAmpHoursDrawn: // H6 mAh
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble);
            double val = aValue.toInt() / 1000.0;
            tag->setValue(val);
            TagSocket *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            tagsocket->hookupTag(tag);
            mTagsockets[aName] = tagsocket;
            break;
        }
        case eTimeSinceFullCharge: // H9 sec
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eInt);
            int val = aValue.toInt();
            tag->setValue(val);
            TagSocket *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eInt);
            tagsocket->hookupTag(tag);
            mTagsockets[aName] = tagsocket;
            break;
        }
        case eAmoutDischargedEnergy: // H17 0.01Kwh
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble);
            double val = aValue.toInt();
            tag->setValue(val);
            TagSocket *socket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            socket->hookupTag(tag);
            mTagsockets[aName] = socket;
            break;
        }
        case eAmountChargedEnergy: // H18 0.01Kwh
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble);
            double val = aValue.toInt();
            tag->setValue(val);
            TagSocket *socket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            socket->hookupTag(tag);
            mTagsockets[aName] = socket;
            break;
        }
        case eNone:
            break;
    }
}


VictronEnergy::Value VictronEnergy::stringToValue(const QString &aString) const
{
    if(aString == "V")
        return eVoltage;
    else if(aString == "I")
        return eAmphere;
    else if(aString == "P")
        return ePower;
    else if(aString == "SOC")
        return eSoc;
    else if(aString == "TTG")
        return eTimeToGo;
    else if(aString == "CE")
        return eConsumedAmpHours;
    else if(aString == "H1")
        return eDepthDepestDischarge;
    else if(aString == "H2")
        return eDepthLastDischarge;
    else if(aString == "H6")
        return eCumulativeAmpHoursDrawn;
    else if(aString == "H9")
        return eTimeSinceFullCharge;
    else if(aString == "H17")
        return eAmoutDischargedEnergy;
    else if(aString == "H18")
        return eAmountChargedEnergy;

    return eNone;
}

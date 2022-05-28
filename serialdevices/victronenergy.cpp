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
            if(mReadFrame.contains("PID") && (mProductName.size() < 2))
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
                    emit intValueReady(iter.key(), value);
                    break;
                }
            case TagSocket::eDouble:
            {
                double value = iter.value().toDouble() / 1000.0;
                tagsocket->writeValue(value);
                emit doubleValueReady(iter.key(), value);
                break;
            }
            case TagSocket::eBool:
            case TagSocket::eString:
            case TagSocket::eTime:
                break;
            }
        }
        else
        {
          createTagSocket(iter.key(), iter.value());
        }
    }
}


/**
 * @brief VictronEnergy::pidToDeviceName
 *
 * Translate the device pid to a human readable name. If no name exist the pid is directly returned
 *          as a hint to the user to add it later. At least all the values recieved from the device is
 *          grouped under the same subsystem.
 *
 * @param aPid
 * @return The product name for the victron device.
 */
QString VictronEnergy::pidToDeviceName(const QString &aPid)
{
    qDebug() << "Pid: " << aPid;
    if(aPid.contains("0XA381"))
        return QString("BMV712_Smart");
    else if(aPid.contains("0XA053"))
        return QString("MPPT75|15");
    else if(aPid.contains("0XA054"))
        return QString("MPPT75|10");
    else if(aPid.contains("0XA055"))
        return QString("MPPT100|15");
    else if(aPid.contains("0XA056"))
        return QString("MPPT100|30");
    else if(aPid.contains("0XA057"))
        return QString("MPPT100|50");
    else if(aPid.contains("0XA058"))
        return QString("MPPT150|35");
    else if(aPid.contains("0XA059"))
        return QString("MPPT150|100rev2");
    else if(aPid.contains("0XA05A"))
        return QString("MPPT150|85rev2");
    else if(aPid.contains("0XA05B"))
        return QString("MPPT250|70");
    else if(aPid.contains("0XA05C"))
        return QString("MPPT250|85");
    else if(aPid.contains("0XA05D"))
        return QString("MPPT250|60");
    else if(aPid.contains("0XA05E"))
        return QString("MPPT250|45");
    else if(aPid.contains("0XA05F"))
        return QString("MPPT100|20");
    else if(aPid.contains("0XA060"))
        return QString("MPPT100|20_48V");
    else if(aPid.contains("0XA061"))
        return QString("MPPT150|45");
    else if(aPid.contains("0XA062"))
        return QString("MPPT150|60");
    else if(aPid.contains("0XA063"))
        return QString("MPPT150|70");

    return aPid;
}

void VictronEnergy::createTagSocket(const QString &aName, const QString &aValue)
{
    if(mProductName.size() < 2)
        return;
    switch (stringToValue(aName))
    {
        case ePower:
        case ePanelPower: //PPV
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eInt);
            int value = aValue.toInt();
            tag->setValue(value);
            TagSocket *tagSocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eInt);
            tagSocket->hookupTag(tag);
            mTagsockets[aName] = tagSocket;
            break;
        }
        case eVoltage:
        case eAmphere:
        case eConsumedAmpHours: // CE, mAh
        case eDepthDepestDischarge: // H1, mAh
        case eDepthLastDischarge: // H2 mAh
        case eCumulativeAmpHoursDrawn: // H6 mAh
        case ePanelVoltage: // VPV mV
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
        case eAmountChargedEnergy: // H18 0.01Kwh
        case eYieldToday: // H20 0.01 KWh
        case eYieldYesterDay: // H22 0.01 KWh
        {
            auto tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble);
            double val = aValue.toDouble() * 10; // W
            tag->setValue(val);
            TagSocket *socket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            socket->hookupTag(tag);
            mTagsockets[aName] = socket;
            break;
        }
        case eMaximumPowerToday: // H21 W
        case eMaximumPowerYesterday: // H23 W
        {
            auto tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eInt);
            int val = aValue.toInt();
            tag->setValue(val);
            TagSocket *socket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            socket->hookupTag(tag);
            mTagsockets[aName] = socket;
            break;
        }
        case eSoc: // SOC
        case eInverterOutAmphere:
        {
            Tag* tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble);
            double val = aValue.toInt() / 10.0;
            tag->setValue(val);
            TagSocket* tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            tagsocket->hookupTag(tag);
            mTagsockets[aName] = tagsocket;
            break;
        }
        case eInverterOutputVoltage:
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble);
            double val = aValue.toDouble() / 100.0;
            tag->setValue(val);
            TagSocket *socket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            socket->hookupTag(tag);
            mTagsockets[aName] = socket;
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
        case eNone:
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eString);
            tag->setValue(aValue);
            TagSocket *socket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eString);
            socket->hookupTag(tag);
            mTagsockets[aName] = socket;
            break;
        }
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
    else if(aString == "H20")
        return eYieldToday;
    else if(aString == "H21")
        return eMaximumPowerToday;
    else if(aString == "H22")
        return eYieldYesterDay;
    else if(aString == "H23")
        return eMaximumPowerYesterday;
    else if(aString == "PPV")
        return ePanelPower;
    else if(aString == "VPV")
        return ePanelVoltage;
    else if(aString == "AC_OUT_V")
        return eInverterOutputVoltage;
    else if(aString == "AC_OUT_I")
        return eInverterOutAmphere;

    return eNone;
}

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
            if(mReadFrame.contains("PID") && !hasPid_)
            {
                QString pid = mReadFrame["PID"];
                mProductName = pidToDeviceName(pid);
                if(hasSerialNumber_)
                    mProductName.append("_" + serialNumber_);
                if(!mProductName.contains("MPPT"))
                    hasSerialNumber_ = true; // serial number NA

                hasPid_ = true;
            }
            if(mReadFrame.contains("SER#") && !hasSerialNumber_)
            {
                auto serialNumber = "_" + mReadFrame["SER#"];
                if(serialNumber.contains("\r"))
                    serialNumber = serialNumber.split("\r").first();
                mProductName.append(serialNumber);

                if(hasPid_)
                    mProductName.append("_" + serialNumber_);

                hasSerialNumber_ = true;
            }

            else if(hasSerialNumber_ && hasPid_)
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
        if(tagsockets_.contains(iter.key()))
        {
            auto tagsocket = tagsockets_[iter.key()];
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
                double value = iter.value().toDouble();
                tagsocket->writeValue(value);
                break;
            }
            case TagSocket::eString:
            {
                tagsocket->writeValue(iter.value());
                break;
            }
            case TagSocket::eBool:
            case TagSocket::eTime:
            case TagSocket::eNone:
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
    else if(aPid.contains("0X203"))
        return "BMV700";
    else if(aPid.contains("0X204"))
        return "BMV702";
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
    else if(aPid.contains("0XA061") || aPid.contains("0XA06A"))
        return QString("MPPT150|45");
    else if(aPid.contains("0XA062"))
        return QString("MPPT150|60");
    else if(aPid.contains("0XA063"))
        return QString("MPPT150|70");

    // Phoenix inverters 250V
    else if(aPid.contains("0XA231"))
        return "Phoenix Inverter 12V 250VA";
    else if(aPid.contains("0XA232"))
        return "Phoenix Inverter 24V 250VA";
    else if(aPid.contains("0XA234"))
        return "Phoenix Inverter 48V 250VA";
    else if(aPid.contains("0XA241"))
        return "Phoenix Inverter 12V 375VA";
    else if(aPid.contains("0XA242"))
        return "Phoenix Inverter 24V 375VA";
    else if(aPid.contains("0XA244"))
        return "Phoenix Inverter 48V 375VA";
    else if(aPid.contains("0XA251"))
        return "Phoenix Inverter 12V 500VA";
    else if(aPid.contains("0XA252"))
        return "Phoenix Inverter 24V 500VA";
    else if(aPid.contains("0XA254"))
        return "Phoenix Inverter 48V 500VA";
    else if(aPid.contains("0XA261"))
        return "Phoenix Inverter 12V 800VA";
    else if(aPid.contains("0XA262"))
        return "Phoenix Inverter 24V 800VA";
    else if(aPid.contains("0XA264"))
        return "Phoenix Inverter 48V 800VA";

    else if(aPid.contains("0XA271"))
        return "Phoenix Inverter 12V 1200VA";
    else if(aPid.contains("0XA272"))
        return "Phoenix Inverter 24V 1200VA";
    else if(aPid.contains("0XA274"))
        return "Phoenix Inverter 48V 1200VA";

    return aPid;
}

void VictronEnergy::createTagSocket(const QString &aName, const QString &aValue)
{
    if(mProductName.size() < 2)
        return;

    auto description = descriptionForValue(stringToValue(aName));
    switch (stringToValue(aName))
    {
        case ePower:
        case ePanelPower: //PPV
        case eMidpointDeviation: // DM %%
        case eTemperature: // T, c
        case eNumberChargeCycles: // H4
        case eNumberFullDischarges: // H5
        case eNumberAutomaticSyncronizations: // H10
        case eNumberAlarmsLowVoltageMainBattery: // H11
        case eNumberAlarmsHighVoltageMainBattery: // H12
        case eNumberAlarmsLowVoltageAuxBattery: // H13
        case eNumberAlarmsHighVoltageAuxBattery: // H14
        case eDay: // HSDS, int
        {
            int value = aValue.toInt();
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eInt, value, description);
            auto *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eInt);
            tagsocket->hookupTag(tag);
            tagsockets_[aName] = tagsocket;
            break;
        }
        case eVoltage:
        case eAmphere:
        case eConsumedAmpHours: // CE, mAh
        case eDepthDepestDischarge: // H1, mAh
        case eDepthLastDischarge: // H2 mAh
        case eCumulativeAmpHoursDrawn: // H6 mAh
        case ePanelVoltage: // VPV mV
        case eVoltageStarterBattery: // VS mV
        case eMidpintVoltage: // VM
        case eLoadCurrent: // IL, mA
        case eDepthAverageDischarge: // H3 maH
        case eMinVoltageMainBattery: //H7 mV
        case eMaxVoltageMainBattery: // H8 mV
        case eMinVoltageAuxBattery: // H15
        case eMaxVoltageAuxBattery: // H16
        {
            double scaleFactor = 0.001;
            double value = aValue.toDouble() * scaleFactor;
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble, value, description);
            TagSocket *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            tagsocket->setScaleValue(scaleFactor);
            tagsocket->hookupTag(tag);
            tagsockets_[aName] = tagsocket;
            break;
        }
        case eTimeSinceFullCharge: // H9 sec
        {
            int value = aValue.toInt();
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eInt, value, description);
            auto *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eInt);
            tagsocket->hookupTag(tag);
            tagsockets_[aName] = tagsocket;
            break;
        }
        case eAmoutDischargedEnergy: // H17 0.01Kwh
        case eAmountChargedEnergy: // H18 0.01Kwh
        case eYieldToday: // H20 0.01 KWh
        case eYieldYesterDay: // H22 0.01 KWh
        case eYieldTotal: // H19 0.01 KwH
        {
            double scaleFactor = 10.0;
            double value = aValue.toDouble() * scaleFactor; // W
            auto tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble, value, description);
            TagSocket *socket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            socket->setScaleValue(scaleFactor);
            socket->hookupTag(tag);
            tagsockets_[aName] = socket;
            break;
        }
        case eMaximumPowerToday: // H21 W
        case eMaximumPowerYesterday: // H23 W
        {
            int value = aValue.toInt();
            auto tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eInt, value, description);
            auto *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eInt);
            tagsocket->hookupTag(tag);
            tagsockets_[aName] = tagsocket;
            break;
        }
        case eSoc: // SOC
        case eInverterOutAmphere:
        {
            double scaleFactor = 0.1;
            double value = aValue.toDouble() * scaleFactor;
            Tag* tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble, value, description);
            TagSocket* tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            tagsocket->setScaleValue(scaleFactor);
            tagsocket->hookupTag(tag);
            tagsockets_[aName] = tagsocket;
            break;
        }
        case eInverterOutputVoltage:
        {
            double scaleFactor = 0.01;
            double value = aValue.toDouble() * scaleFactor;
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eDouble, value, description);
            TagSocket *socket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eDouble);
            socket->setScaleValue(scaleFactor);
            socket->hookupTag(tag);
            tagsockets_[aName] = socket;
            break;
        }
        case eTimeToGo: //TTG
        {
            int value = aValue.toInt();
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eInt, value, description);
            auto *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eInt);
            tagsocket->hookupTag(tag);
            tagsockets_[aName] = tagsocket;
            break;
        }
        case eAlarm: // alarm
        case eRelay: // Relay
        case eErrorCode: // ERR
        case eFirmwareVersioin: // FW, string
        case eSerialNumber: // #SER, string
        {
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eString, aValue, description);
            auto *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eString);
            tagsocket->hookupTag(tag);
            tagsockets_[aName] = tagsocket;
            break;
        }
        case eStateOfOperation: // CS
        {
            QString value = StateOfOperationToString(aValue.toInt());
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eString, value, description);
            auto *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eString);
            tagsocket->hookupTag(tag);
            tagsockets_[aName] = tagsocket;
            break;
        }
        case eDeviceMode: // MODE, int
        {
            auto value = deviceModeToString(aValue.toInt());
            Tag *tag = TagList::sGetInstance().createTag(mProductName, aName, Tag::eString, value, description);
            auto *tagsocket = TagSocket::createTagSocket(mProductName, aName, TagSocket::eString);
            tagsocket->hookupTag(tag);
            tagsockets_[aName] = tagsocket;
            break;
        }
        case eNone:
        {
            break;
        }
    }
}

QString VictronEnergy::descriptionForValue(Value value)
{
    switch (value) {
        case eVoltage: // V
            return "Main battery voltage";
        case eAmphere: // I
            return "Battery current";
        case ePower: // P
            return "Instantaneous power";
        case eSoc: // SOC
            return "State-of-charge";
        case eTimeToGo: //TTG
            return "Time-to-go";
        case eConsumedAmpHours: // CE, mAh
            return "Consumed Amp hours";
        case eDepthDepestDischarge: // H1, mAh
            return "Depth of the deepest discharge";
        case eDepthLastDischarge: // H2 mAh
            return "Depth of last discharge";
        case eCumulativeAmpHoursDrawn: // H6 mAh
            return "Cumulative Amp hours drawn";
        case eTimeSinceFullCharge: // H9 sec
            return "Number of seconds since last full charge";
        case eAmoutDischargedEnergy: // H17 0.01Kwh
            return "Amount of discharged energy";
        case eAmountChargedEnergy: // H18 0.01Kwh
            return "Amount of charged energy";
        case eYieldToday: // H20 0.01 KWh
            return "Yield today";
        case eMaximumPowerToday: // H21 W
            return "Maximum power today";
        case eYieldYesterDay: // H22 0.01 KWh
            return "Yield yesteday";
        case eMaximumPowerYesterday: // H23 W
            return "Maximum power yesterday";
        case ePanelPower: // PPV mV
            return "Panel power";
        case ePanelVoltage: // VPV mV
            return "Panel voltage";
        case eInverterOutputVoltage: // AC_OUT_V 0.01V
            return "AC output voltage";
        case eInverterOutAmphere: // AC_OUT_I 0.1A
            return "AC output power";

        case eVoltageStarterBattery:
            return "Voltage Aux battery";
        case eMidpintVoltage:
            return "Mid-point Voltage Battery";
        case eMidpointDeviation:
            return "Mid-point deviation Battery";
        case eLoadCurrent:
            return "Load Current";
        case eTemperature:
            return "Battery temperature";
        case eAlarm:
            return "Alarm condition active";
        case eRelay:
            return "Relay state";
        case eDepthAverageDischarge:
            return "Depth of the average discharge";
        case eNumberChargeCycles:
            return "Number of charge cycles";
        case eNumberFullDischarges:
            return "Number of full discharges";
        case eMinVoltageMainBattery:
            return "Minimum Voltage Battery";
        case eMaxVoltageMainBattery:
            return "Maximum Voltage Battery";
        case eNumberAutomaticSyncronizations:
            return "Number og automatic syncronizations";
        case eNumberAlarmsLowVoltageMainBattery:
            return "Number of low Voltage Alarms, Battery";
        case eNumberAlarmsHighVoltageMainBattery:
            return "Number of high Voltage Alarms, Battery";
        case eNumberAlarmsLowVoltageAuxBattery:
            return "Number of low Voltage Alarms, Aux Battery";
        case eNumberAlarmsHighVoltageAuxBattery:
            return "Number of high Voltage Alarms, Aux Battery";
        case eMinVoltageAuxBattery:
            return "Minimum Voltage Aux Battery";
        case eMaxVoltageAuxBattery:
            return "Maximum Voltage Aux Battery";
        case eYieldTotal:
            return "Yield total";
        case eErrorCode:
            return "Error code";
        case eStateOfOperation:
            return "State of operation";
        case eFirmwareVersioin:
            return "Firmware version";
        case eSerialNumber:
            return "Serial number";
        case eDay:
            return "Day (0-364)";
        case eDeviceMode:
            return "Device mode";
    default:
        return QString();
    }
}

QString VictronEnergy::StateOfOperationToString(int cs)
{
    if(cs == 0)
        return "Off";
    else if(cs == 1)
        return "Low power";
    else if(cs == 2)
        return "Fault";
    else if(cs == 3)
        return "Bulk";
    else if(cs == 4)
        return "Absorption";
    else if(cs == 5)
        return "Float";
    else if(cs == 9)
        return "Inverting";
    else
        return "Unknown state";

}

QString VictronEnergy::deviceModeToString(int mode)
{
    if(mode == 2)
        return "MODE INVERTER";
    else if(mode == 4)
        return "MODE OFF";
    else if(mode == 5)
        return "MODE ECO";
    else
        return QString();
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
    else if(aString == "VS")
        return eVoltageStarterBattery; // VS mV
    else if(aString == "VM")
        return eMidpintVoltage; // VM
    else if(aString == "DM")
        return eMidpointDeviation; // DM %%
    else if(aString == "IL")
        return eLoadCurrent; // IL, mA
    else if(aString == "T")
        return eTemperature; // T, c
    else if(aString == "Alarm")
        return eAlarm; // alarm
    else if(aString == "Relay")
        return eRelay; // Relay
    else if(aString == "H3")
        return eDepthAverageDischarge; // H3 maH
    else if(aString == "H4")
        return eNumberChargeCycles; // H4
    else if(aString == "H5")
        return eNumberFullDischarges; // H5
    else if(aString == "H7")
        return eMinVoltageMainBattery; //H7 mV
    else if(aString == "H8")
        return eMaxVoltageMainBattery; // H8 mV
    else if(aString == "H10")
        return eNumberAutomaticSyncronizations; // H10
    else if(aString == "H11")
        return eNumberAlarmsLowVoltageMainBattery; // H11
    else if(aString == "H12")
        return eNumberAlarmsHighVoltageMainBattery; // H12
    else if(aString == "H13")
        return eNumberAlarmsLowVoltageAuxBattery; // H13
    else if(aString == "H14")
        return eNumberAlarmsHighVoltageAuxBattery; // H14
    else if(aString == "H15")
        return eMinVoltageAuxBattery; // H15
    else if(aString == "H16")
        return eMaxVoltageAuxBattery; // H16
    else if(aString == "H19")
        return eYieldTotal; // H19 0.01 KwH
    else if(aString == "ERR")
        return eErrorCode; // ERR
    else if(aString == "CS")
        return eStateOfOperation; // CS
    else if(aString =="FW")
        return eFirmwareVersioin; // FW, string
    else if(aString =="SER#")
        return eSerialNumber; // #SER, string
    else if(aString =="HSDS")
        return eDay; // HSDS, int
    else if(aString =="MODE")
        return eDeviceMode; // MODE, int

    return eNone;
}

#include "bmw712smart.h"

#include <QStringList>
#include <QByteArray>
#include <QString>
#include <QDataStream>
#include <QDebug>

#include <tagsystem/taglist.h>
#include <tagsystem/tag.h>

Bmw712Smart::Bmw712Smart() : Device()
{
    qDebug() << __FUNCTION__;
    mVoltageTag = TagList::sGetInstance().createTag("bmv", "voltage", Tag::eDouble);
    mAmpTag = TagList::sGetInstance().createTag("bmv", "amp", Tag::eDouble);
    mSocTag = TagList::sGetInstance().createTag("bmv", "soc", Tag::eDouble);
    mTtgTag = TagList::sGetInstance().createTag("bmv", "ttg", Tag::eInt);
    mLastDischargeTag = TagList::sGetInstance().createTag("bmv", "lastDischarge", Tag::eDouble);
    mDischargedEnergyTag = TagList::sGetInstance().createTag("bmv", "dischargedEnergy", Tag::eDouble);
    mChargedEnergyTag = TagList::sGetInstance().createTag("bmv", "chargedEnergy", Tag::eDouble);
    mBatteryPowerTag = TagList::sGetInstance().createTag("bmv", "power", Tag::eDouble);
}

/**
 * @brief Bmw712Smart::dataRead
 * @param aData
 *
 *  Available data from documentation.
 *
 *  V   (mV) Main battery voltage
 *  VS  (mV) Starter battery voltage
 *  VM  (mV) Mid point voltage
 *  DM  (promile) mid point deviation
 *  I   (mA) battery current
 *  T   (C) battery temperature
 *  P   (W) Instancety power
 *  CE  (mAh) Consumed amp hours
 *  SOC (promile) state of charge
 *  TTG (Minutes) Time to go
 *  ALARM       Alarm condition active
 *  RELAY       Relay state
 *  AR          Alarm reason
 *  H1  (mAh)   Depth of deepest discharge
 *  H2  (mAh)   Depth of last discharge
 *  H3  (mAh)   Depth og average discharge
 *  H4          Number of charge cycles
 *  H5          Number of full discharges
 *  H6  (mAh)   Cumulative amp hours drawn
 *  H7  (mV)    Min main (battery) voltage
 *  H8  (mV)    Max main (battery) voltage
 *  H9  (S)     Number of seconds since last full charge
 *  H10         Number of automatic syncronization
 *  H11
 *  H12
 *  H15
 *  H16
 *  H17 (0.01kWh) Amout of discharged energy
 *  H18 (0.01kWh) Amout of charged energy
 */
void Bmw712Smart::dataRead(QByteArray aData)
{
    if(!isValid(aData))
    {
        return;
    }
    aData.remove(aData.size()-1, 1); // remove checksum
    /*if(mBuffer.size() > 500)
    {
        mBuffer.clear();
        return;
    }

    mBuffer.append(aData);

    QByteArray dataBlock = getDataBlock(mBuffer);
    if(dataBlock.isEmpty())
        return;*/


    QStringList messages = QString(aData).split("\n");
    for(int i=0; i<messages.size(); ++i)
    {
        if(messages[i].startsWith("VS"))
        {

        }
        else if(messages[i].startsWith("VM"))
        {

        }
        else if(messages[i].startsWith("V"))
        {
            QString data = messages[i].split("\t").last();
            if(data.isEmpty())
                continue;
            if(data.size() == 4)
                data.push_back("0");
            else if(data.size() == 3)
                data.push_back("00");
            else if(data.size() == 2)
                data.push_back("000");
            bool ok;
            int v = data.toInt(&ok);
            if( (v/1000.0) < 12.0)
                qDebug();
            if(ok)
                setMainBatteryVoltage(v/1000.0);
        }
        else if(messages[i].startsWith("I"))
        {
            QString data = messages[i].split("\t").last();
            if(data.size() == 4)
                data.push_back("0");
            else if(data.size() == 3)
                data.push_back("00");
            else if(data.size() == 2)
                data.push_back("000");
            bool ok;
            int i = data.toInt(&ok);

            if(ok)
                setMainBatteryCurrent(i/1000.0);
        }
        else if(messages[i].startsWith("P"))
        {
            QString data = messages[i].split("\t").last();
            bool ok;
            int p = data.toInt(&ok);
            if(ok)
                setBatteryPower(p);
        }
        else if(messages[i].startsWith("CE"))
        {

        }
        else if(messages[i].startsWith("SOC"))
        {
            QString data = messages[i].split("\t").last();
            bool ok;
            int soc = data.toInt(&ok);
            if(ok)
                setStateOfCharge(soc);
        }
        else if(messages[i].startsWith("TTG"))
        {
            QString data = messages[i].split("\t").last();
            bool ok;
            int ttg = data.toInt(&ok);
            if(ok)
                setTimeToGo(ttg);
        }
        else if(messages[i].startsWith("H17"))
        {
            QString data = messages[i].split("\t").last();
            bool ok;
            int discharged = data.toInt(&ok);
            if(ok)
                setDischargedEnergy(discharged);
        }
        else if(messages[i].startsWith("H18"))
        {
            QString data = messages[i].split("\t").last();
            bool ok;
            int charged = data.toInt(&ok);
            if(ok)
                setChargedEnergy(charged);
        }
        else if(messages[i].startsWith("H1"))
        {

        }
        else if(messages[i].startsWith("H2"))
        {
            QString data = messages[i].split("\t").last();
            bool ok;
            int last = data.toInt(&ok);
            if(ok)
                setLastDischarge(last);
        }
        else if(messages[i].startsWith("H6"))
        {

        }
        else if(messages[i].startsWith("H9"))
        {

        }

    }
}


void Bmw712Smart::setMainBatteryVoltage(double aV)
{
 //   qDebug() << __FUNCTION__ << "Battery voltage: " << aV;
    mMainBatteryVoltage = aV;
    mVoltageTag->setValue(mMainBatteryVoltage);
    emit mainBatteryVoltageChanged(mMainBatteryVoltage);
}


void Bmw712Smart::setMainBatteryCurrent(double aI)
{
  //  qDebug() << __FUNCTION__ << "Battery current: " << aI << "A";
    mMainBAtteryCurrent = aI;
    mAmpTag->setValue(mMainBAtteryCurrent);
    emit mainBatteryCurrentChanged(mMainBAtteryCurrent);
}


void Bmw712Smart::setBatteryPower(double aP)
{
    mBatteryPower = aP;
    mBatteryPowerTag->setValue(aP);
    emit batteryPowerChanged(mBatteryPower);
}


void Bmw712Smart::setConsumedAmpHours(double aAmpH)
{
    mConsumedAmpHours = aAmpH;
    emit consumedAmpHoursChanged(mConsumedAmpHours);
}


void Bmw712Smart::setStateOfCharge(double aSoc)
{
    mSOC = aSoc;
    mSocTag->setValue(aSoc);
    emit stateOfChargeChanged(mSOC);
}


void Bmw712Smart::setTimeToGo(double aTtg)
{
    mTTG = aTtg;
    mTtgTag->setValue((int)aTtg);
    emit timeToGoChanged(mTTG);
}


void Bmw712Smart::setDeepestDischarge(double aAmpH)
{
    mDeepestDischarge = aAmpH;
    emit deepestDischargedChanged(mDeepestDischarge);
}


void Bmw712Smart::setLastDischarge(double aAmpH)
{
    mLastDischarge = aAmpH;
    mLastDischargeTag->setValue(aAmpH);
    emit lastDischargeChanged(mLastDischarge);
}


void Bmw712Smart::setCumulativeAmpHoursDrawn(double aAmpH)
{
    mCumulativeAmpHoursDrawn = aAmpH;
    emit cumulativeAmpHoursChanged(mCumulativeAmpHoursDrawn);
}


void Bmw712Smart::setNumberSecondsSinceLastFullCharge(int aS)
{
    mNumberOfSecondsSinceLastFullCharge = aS;
    emit numberSecondsSinceLastFullChargeChanged(mNumberOfSecondsSinceLastFullCharge);
}


void Bmw712Smart::setDischargedEnergy(double aKwh)
{
    mDischargedEnergy = aKwh;
    mDischargedEnergyTag->setValue(aKwh);
    emit dischargedEnergyChanged(mDischargedEnergy);
}


void Bmw712Smart::setChargedEnergy(double aKwh)
{
    mChargedEnergy = aKwh;
    mChargedEnergyTag->setValue(aKwh);
    emit chargedEnergyChanged(mChargedEnergy);
}

QByteArray Bmw712Smart::getDataBlock(QByteArray &aBuffer)
{
    QByteArray dataBlock;
    int sum = 0;
    for(int i=0; i<aBuffer.size(); ++i)
    {
        sum += (int)aBuffer.at(i);

        int r = sum % 256;
        if(r == 0)
        {
            aBuffer.remove(0, i);
            return dataBlock;
        }
        dataBlock.push_back(aBuffer.at(i));
    }

    return QByteArray();
}

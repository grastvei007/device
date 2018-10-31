#ifndef BMW712SMART_H
#define BMW712SMART_H

#include "../device.h"

class Tag;

/**
 * @brief The Bmw712Smart class
 *
 * Reads the values from victron energy bmv712 smart over VE.direct
 * protocol. The values is made available on Tags.
 *
 *
 */
class Bmw712Smart : public Device
{
    Q_OBJECT
public:
    Bmw712Smart();

    int getPid() const {return 825766721;}

    void dataRead(QByteArray aData); ///< parse input data.

signals:
    void mainBatteryVoltageChanged(double);
    void mainBatteryCurrentChanged(double);
    void batteryPowerChanged(double);
    void consumedAmpHoursChanged(double);
    void stateOfChargeChanged(double);
    void timeToGoChanged(double);
    void deepestDischargedChanged(double);
    void lastDischargeChanged(double);
    void cumulativeAmpHoursChanged(double);
    void numberSecondsSinceLastFullChargeChanged(int);
    void dischargedEnergyChanged(double);
    void chargedEnergyChanged(double);

protected:
    void setMainBatteryVoltage(double aV);
    void setMainBatteryCurrent(double aI);
    void setBatteryPower(double aP);
    void setConsumedAmpHours(double aAmpH);
    void setStateOfCharge(double aSoc);
    void setTimeToGo(double aTtg);
    void setDeepestDischarge(double aAmpH);
    void setLastDischarge(double aAmpH);
    void setCumulativeAmpHoursDrawn(double aAmpH);
    void setNumberSecondsSinceLastFullCharge(int aS);
    void setDischargedEnergy(double aKwh);
    void setChargedEnergy(double aKwh);

    QByteArray getDataBlock(QByteArray &aBuffer);
private:
    QByteArray mBuffer;

    double mMainBatteryVoltage;
    double mMainBAtteryCurrent;
    double mBatteryPower;
    double mConsumedAmpHours;
    double mSOC;
    double mTTG;
    double mDeepestDischarge;
    double mLastDischarge;
    double mCumulativeAmpHoursDrawn;
    int mNumberOfSecondsSinceLastFullCharge;
    double mDischargedEnergy;
    double mChargedEnergy;

    // tags
    Tag *mVoltageTag;
    Tag *mAmpTag;
    Tag *mSocTag;
    Tag *mTtgTag;
    Tag *mLastDischargeTag;
    Tag *mDischargedEnergyTag;
    Tag *mChargedEnergyTag;
    Tag *mBatteryPowerTag;
};

#endif // BMW712SMART_H

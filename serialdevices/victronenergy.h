#ifndef VICTRONENERGY_H
#define VICTRONENERGY_H

#include <QObject>
#include <QByteArray>
#include <QMap>

#include "../device.h"

#include <tagsystem/tagsocket.h>

class VictronEnergy : public Device
{
    Q_OBJECT
public:
    enum State
    {
        eIdle,
        eBegin,
        eName,
        eValue,
        eChecksum
    };
    enum Value ///< values parsed from victron device.
    {
        eNone,
        eVoltage, // V
        eAmphere, // I
        ePower, // P
        eSoc, // SOC
        eTimeToGo, //TTG
        eConsumedAmpHours, // CE, mAh
        eDepthDepestDischarge, // H1, mAh
        eDepthLastDischarge, // H2 mAh
        eCumulativeAmpHoursDrawn, // H6 mAh
        eTimeSinceFullCharge, // H9 sec
        eAmoutDischargedEnergy, // H17 0.01Kwh
        eAmountChargedEnergy, // H18 0.01Kwh
        eYieldToday, // H20 0.01 KWh
        eMaximumPowerToday, // H21 W
        eYieldYesterDay, // H22 0.01 KWh
        eMaximumPowerYesterday, // H23 W
        ePanelPower, // PPV mV
        ePanelVoltage, // VPV mV
        eInverterOutputVoltage, // AC_OUT_V 0.01V
        eInverterOutAmphere // AC_OUT_I 0.1A
    };

    VictronEnergy();

    int getPid() const override;
    void dataRead(QByteArray aData) override;
    Value stringToValue(const QString &aString) const;

private:
    void recordFrame(char c);
    void putValuesOnTags();
    QString pidToDeviceName(const QString &aPid);
    void createTagSocket(const QString &aName, const QString &aValue);

    int mChecksum;
    QByteArray mFrame;
    State mState;
    QString mName;
    QString mValue;
    QMap<QString, QString> mReadFrame;
    QMap<QString, TagSocket*> mTagsockets;
    QString mProductName;
};

#endif // VICTRONENERGY_H

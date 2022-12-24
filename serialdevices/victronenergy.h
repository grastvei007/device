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
        eInverterOutAmphere, // AC_OUT_I 0.1A
        eVoltageStarterBattery, // VS mV
        eMidpintVoltage, // VM
        eMidpointDeviation, // DM %%
        eLoadCurrent, // IL, mA
        eTemperature, // T, c
        eAlarm, // alarm
        eRelay, // Relay
        eDepthAverageDischarge, // H3 maH
        eNumberChargeCycles, // H4
        eNumberFullDischarges, // H5
        eMinVoltageMainBattery, //H7 mV
        eMaxVoltageMainBattery, // H8 mV
        eNumberAutomaticSyncronizations, // H10
        eNumberAlarmsLowVoltageMainBattery, // H11
        eNumberAlarmsHighVoltageMainBattery, // H12
        eNumberAlarmsLowVoltageAuxBattery, // H13
        eNumberAlarmsHighVoltageAuxBattery, // H14
        eMinVoltageAuxBattery, // H15
        eMaxVoltageAuxBattery, // H16
        eYieldTotal, // H19 0.01 KwH
        eErrorCode, // ERR
        eStateOfOperation, // CS
        eFirmwareVersioin, // FW, string
        eSerialNumber, // #SER, string
        eDay, // HSDS, int
        eDeviceMode, // MODE, int
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
    QString descriptionForValue(Value value);
    QString StateOfOperationToString(int cs);
    QString deviceModeToString(int mode);

    int mChecksum;
    bool hasSerialNumber_ = false;
    bool hasPid_ = false;
    QString serialNumber_;

    QByteArray mFrame;
    State mState;
    QString mName;
    QString mValue;
    QMap<QString, QString> mReadFrame;
    QMap<QString, TagSocket*> mTagsockets;
    QString mProductName;
};

#endif // VICTRONENERGY_H

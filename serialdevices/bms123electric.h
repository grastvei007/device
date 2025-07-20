#ifndef BMS123ELECTRIC_H
#define BMS123ELECTRIC_H

#include <QObject>

#include "../device.h"
#include <map>

class Tag;

class BMS123electric : public Device
{
    Q_OBJECT
public:
    BMS123electric(QObject *parent = nullptr);

    void dataRead(QByteArray data) override;
    int getPid() const override;

private:
    const QString deviceName_{"123SmartBms"};
    struct cellTags
    {
        Tag* voltage = nullptr;
        Tag* temperature = nullptr;
    };
    struct Status{
        Tag* allowToCharge = nullptr;
        Tag* allowToDischarge = nullptr;
        Tag* commError = nullptr;
        Tag* exceedVmin = nullptr;
        Tag* exceedVmax = nullptr;
        Tag* exceedTmin = nullptr;
        Tag* exceedTmax = nullptr;
        Tag* socNotCalibrated = nullptr;
    };

    void processFrame();
    bool isChecksumValid();
    QByteArray frame_;

    double readTotalVoltage();
    int readCurrentCell();
    double readCellVoltage(int cell);
    int readCellTemperature(int cell);
    void readStatusByte1(); ///< alarms

    std::map<int, cellTags> cellTags_;
    Tag* totalVoltageTag_;
    Status statusTags_;
};

#endif // BMS123ELECTRIC_H

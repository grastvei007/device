#include "bms123electric.h"

#include <tagsystem/tag.h>
#include <tagsystem/taglist.h>

#include <QDebug>

#include <QBitArray>
#include <bitset>

union convert
{
    int i;
    uint8_t buffer[4];
};

BMS123electric::BMS123electric(QObject *parent) :
    Device(parent)
{
    totalVoltageTag_ = TagList::sGetInstance().createTag(deviceName_, "total_voltage", TagType::eDouble, 0.0, "Totale voltage of battery");

    statusTags_.allowToCharge = TagList::sGetInstance().createTag(deviceName_, "allow_to_charge", TagType::eBool, true, "Allow to charge battery");
    statusTags_.allowToDischarge = TagList::sGetInstance().createTag(deviceName_, "allow_to_discharge", TagType::eBool, true, "Allow to discharge battery");
    statusTags_.commError = TagList::sGetInstance().createTag(deviceName_, "comm_error", TagType::eBool, false, "Communication error");
    statusTags_.exceedVmin = TagList::sGetInstance().createTag(deviceName_, "exceed_v_min", TagType::eBool, false, "Exceed min voltage");
    statusTags_.exceedVmax = TagList::sGetInstance().createTag(deviceName_, "exceed_v_max", TagType::eBool, false, "Exceed max voltage");
    statusTags_.exceedTmin = TagList::sGetInstance().createTag(deviceName_, "exceed_t_min", TagType::eBool, false, "Exceed min temperature");
    statusTags_.exceedTmax = TagList::sGetInstance().createTag(deviceName_, "exceed_t_max", TagType::eBool, false, "Exceed max temperature");
    statusTags_.socNotCalibrated = TagList::sGetInstance().createTag(deviceName_, "soc_not_calibrated", TagType::eBool, false, "soc not calibrated");

    // cells are created on demand to allow use for different batteries.
}

void BMS123electric::dataRead(QByteArray data)
{
    frame_.append(data);
    if(frame_.size() == 58)
    {
        // process data...
        processFrame();
        frame_.clear();
    }
    else if(frame_.size() > 58)
    {
        qDebug() << "Error deleting frame";
        frame_.clear();
    }
}

int BMS123electric::getPid() const
{
    return 37;
}

void BMS123electric::processFrame()
{
    if(!isChecksumValid())
        return;

    readTotalVoltage();
    auto currentCell = readCurrentCell();

    readCellVoltage(currentCell);
    readCellTemperature(currentCell);
    readStatusByte1();
}

bool BMS123electric::isChecksumValid()
{
    int checksum = (uint8_t)frame_.at(57);
    int sum = 0;
    for(int i= 0; i<57; ++i)
        sum += (uint8_t)frame_.at(i);
    int low = sum & 0xFF;

    return checksum == low;
}

double BMS123electric::readTotalVoltage()
{
    QString hex(frame_.mid(0, 3).toHex());
    bool v;
    double val = hex.toInt(&v, 16) * 0.005;

    totalVoltageTag_->setValue(val);

    return val;
}

int BMS123electric::readCurrentCell()
{
    QString hex(frame_.mid(24, 1).toHex());
    bool v;
    int val = hex.toInt(&v, 16);
    return val;
}

double BMS123electric::readCellVoltage(int cell)
{
    QString hex(frame_.mid(26, 2).toHex());
    bool v;
    double val = hex.toInt(&v, 16) * 0.005;

    if(!cellTags_.contains(cell))
    {
        cellTags tags;
        tags.voltage = TagList::sGetInstance().createTag(deviceName_, QString("cell_%1_voltage").arg(cell), TagType::eDouble, 0.0, QString("cell %1 voltage").arg(cell));
        tags.temperature = TagList::sGetInstance().createTag(deviceName_, QString("cell_%1_temperature").arg(cell), TagType::eInt, 0, QString("cell %1 temperature").arg(cell));
        cellTags_[cell] = tags;
    }

    cellTags_[cell].voltage->setValue(val);

    return val;
}

int BMS123electric::readCellTemperature(int cell)
{
    QString hex(frame_.mid(28, 2).toHex());
    bool v;
    int val = hex.toInt(&v, 16) - 276;

    if(!cellTags_.contains(cell))
    {
        cellTags tags;
        tags.voltage = TagList::sGetInstance().createTag(deviceName_, QString("cell_%1_voltage").arg(cell), TagType::eDouble, 0.0, QString("cell %1 voltage").arg(cell));
        tags.temperature = TagList::sGetInstance().createTag(deviceName_, QString("cell_%1_temperature").arg(cell), TagType::eInt, 0, QString("cell %1 temperature").arg(cell));
        cellTags_[cell] = tags;
    }

    cellTags_[cell].temperature->setValue(val);

    return val;
}

void BMS123electric::readStatusByte1()
{
    auto status = (uint8_t)frame_.at(30);
    auto n = QString::number(status, 16).toInt();

    std::bitset<8> bitset(n);

    statusTags_.allowToCharge->setValue(static_cast<bool>(bitset[0]));
    statusTags_.allowToDischarge->setValue(static_cast<bool>(bitset[1]));
    statusTags_.commError->setValue(static_cast<bool>(bitset[2]));
    statusTags_.exceedVmin->setValue(static_cast<bool>(bitset[3]));
    statusTags_.exceedVmax->setValue(static_cast<bool>(bitset[4]));
    statusTags_.exceedTmin->setValue(static_cast<bool>(bitset[5]));
    statusTags_.exceedTmax->setValue(static_cast<bool>(bitset[6]));
    statusTags_.socNotCalibrated->setValue(static_cast<bool>(bitset[7]));
}

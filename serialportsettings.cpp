#include "serialportsettings.h"

SerialPortSettings::SerialPortSettings()
{

    mBaudRate = QSerialPort::Baud9600;
    mDataBits = QSerialPort::Data8;
    mParity = QSerialPort::NoParity;
    mStopBits = QSerialPort::OneStop;
    mFlowControl =QSerialPort::NoFlowControl;

}


SerialPortSettings::SerialPortSettings(const SerialPortSettings &aSerialPortSettings) :
    mName(aSerialPortSettings.mName),
    mBaudRate(aSerialPortSettings.mBaudRate),
    mStringBaudRate(aSerialPortSettings.mStringBaudRate),
    mDataBits(aSerialPortSettings.mDataBits),
    mStringDataBits(aSerialPortSettings.mStringDataBits),
    mParity(aSerialPortSettings.mParity),
    mStringParity(aSerialPortSettings.mStringParity),
    mStopBits(aSerialPortSettings.mStopBits),
    mStringStopBits(aSerialPortSettings.mStringStopBits),
    mFlowControl(aSerialPortSettings.mFlowControl),
    mStringFlowControl(aSerialPortSettings.mStringFlowControl),
    mLocalEchoEnabled(aSerialPortSettings.mLocalEchoEnabled)
{
}


void SerialPortSettings::setName(const QString& aName)
{
    mName = aName;
}


void SerialPortSettings::setBaudRate(qint32 aBaudRate)
{
    mBaudRate = aBaudRate;
}


void SerialPortSettings::setStringBaudRate(const QString& aStringBaudRate)
{
    mStringBaudRate = aStringBaudRate;
}


void SerialPortSettings::setDataBits(QSerialPort::DataBits aDataBits)
{
    mDataBits = aDataBits;
}


void SerialPortSettings::setStringDataBits(const QString &aStringDataBits)
{
    mStringDataBits = aStringDataBits;
}


void SerialPortSettings::setParity(QSerialPort::Parity aParity)
{
    mParity = aParity;
}


void SerialPortSettings::setStringParity(const QString &aStringParity)
{
    mStringParity = aStringParity;
}


void SerialPortSettings::setStopBits(QSerialPort::StopBits aStopBits)
{
    mStopBits = aStopBits;
}


void SerialPortSettings::setStringStopBits(const QString &aStringStopBits)
{
    mStringStopBits = aStringStopBits;
}


void SerialPortSettings::setFlowControl(QSerialPort::FlowControl aFlowControl)
{
    mFlowControl = aFlowControl;
}


void SerialPortSettings::setStringFlowControl(const QString& aStringFlowControl)
{
    mStringFlowControl = aStringFlowControl;
}


void SerialPortSettings::setLocalEchoEnabled(bool aLocalEchoEnabled)
{
    mLocalEchoEnabled = aLocalEchoEnabled;
}


const QString& SerialPortSettings::getName() const
{
    return mName;
}


qint32 SerialPortSettings::getBaudRate() const
{
    return mBaudRate;
}

const QString& SerialPortSettings::getStringBaudRate() const
{
    return mStringBaudRate;
}


QSerialPort::DataBits SerialPortSettings::getDataBits() const
{
    return mDataBits;
}


const QString& SerialPortSettings::getStringDataBits() const
{
    return mStringDataBits;
}


QSerialPort::Parity SerialPortSettings::getParity() const
{
    return mParity;
}


const QString& SerialPortSettings::getStringParity() const
{
    return mStringParity;
}


QSerialPort::StopBits SerialPortSettings::getStopBits() const
{
    return mStopBits;
}


const QString& SerialPortSettings::getStringStopBits() const
{
    return mStringStopBits;
}


QSerialPort::FlowControl SerialPortSettings::getFlowControl() const
{
    return mFlowControl;
}


const QString& SerialPortSettings::stringFlowControl() const
{
    return mStringFlowControl;
}


bool SerialPortSettings::getLocalEchoEnabled() const
{
    return mLocalEchoEnabled;
}

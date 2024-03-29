#include "inputdevicemanager.h"

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QDebug>

#include "serialportsettings.h"
#include "serialdevices/victronenergy.h"
#include "serialdevices/atmega.h"



InputDeviceManager::InputDeviceManager() :
    mDefaultSerialPortSettings(),
    mUseDefaultSerialPortSettingFlag(false),
    mPollingIntervalMs(1000),
    mPidExtractDevice(nullptr),
    mPidErrorTimer(nullptr)
{
    mPollingIntervalTimer.reset(new QTimer());
    mPollingIntervalTimer->setInterval(mPollingIntervalMs);
    connect(mPollingIntervalTimer.get(), &QTimer::timeout, this, &InputDeviceManager::detectInputDevices);
    //mPollingIntervalTimer->start();

}

void InputDeviceManager::setDetectInputDevicesInterval(int aIntervalInMs)
{
    mPollingIntervalTimer->stop();
    mPollingIntervalMs = aIntervalInMs;
    mPollingIntervalTimer->setInterval(mPollingIntervalMs);
    mPollingIntervalTimer->start();
}


void InputDeviceManager::setUseDefaultSerialSettingFlag(bool aBool)
{
    mUseDefaultSerialPortSettingFlag = aBool;
}


void InputDeviceManager::setDefaultBaudRate(BaudRate aBaudRate)
{
    if(aBaudRate == e9600)
        mDefaultSerialPortSettings.setBaudRate(9600);
    else if(aBaudRate == e19200)
        mDefaultSerialPortSettings.setBaudRate(19200);
    else if(aBaudRate == e38400)
        mDefaultSerialPortSettings.setBaudRate(38400);
    else if(aBaudRate == e115200)
        mDefaultSerialPortSettings.setBaudRate(115200);
}


int InputDeviceManager::getDefaultBaudRate() const
{
    return mDefaultSerialPortSettings.getBaudRate();
}

void InputDeviceManager::detectInputDevices()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    QList<QString> portNames;
    for(auto i =ports.begin(); i != ports.end(); ++i)
    {
        portNames.push_back(i->portName());
    }

    if(mAvailableSerialPorts.isEmpty())
    {
        // make all ports available.
        for(auto &portName : portNames)
        {
            mAvailableSerialPorts.push_back(portName);
            emit inputDeviceAvailable(portName);
        }
        return;
    }

    // test to se if some ports has changed add/remove.
    for(auto &port : portNames)
    {
        if(!mAvailableSerialPorts.contains(port))
        {
            mAvailableSerialPorts.push_back(port);
            emit inputDeviceAvailable(port);
        }
    }
    QStringList remove;
    for(auto &availablePort : mAvailableSerialPorts)
    {
        if(!portNames.contains(availablePort))
            remove.push_back(availablePort);
    }

    // remove and disconnect from serial port
    for(auto &deviceName : remove)
    {
        mAvailableSerialPorts.removeAll(deviceName);
        if(mConnectedInputDevices.contains(deviceName))
        {
            mConnectedInputDevices[deviceName]->deleteLater();
            mConnectedInputDevices.remove(deviceName);
        }
        emit inputDeviceDisconnected(deviceName);
    }

}


void InputDeviceManager::connectInputDevice(QString aDeviceName)
{
    if(!mUseDefaultSerialPortSettingFlag)
    {
        qDebug() << __FUNCTION__ << " Use default serial port settings is not set, set flag or call function with serialport settings";
        return;
    }

    if(mConnectedInputDevices.contains(aDeviceName))
        return;

    if(getInputDevice(aDeviceName)) // already connected.
        return;

    SerialPortSettings *settings = new SerialPortSettings(mDefaultSerialPortSettings);
    settings->setName(aDeviceName);

    QSerialPort serialport;
    serialport.setPortName(aDeviceName);
    QSerialPortInfo spi(serialport);
    QString manufacturer;
    int id = spi.vendorIdentifier();
    if(id > 0)
        manufacturer = spi.manufacturer();
    if(manufacturer.contains("VictronEnergy"))
    {
        settings->setBaudRate(19200);
        VictronEnergy *ve = new VictronEnergy();
        ve->openDevice(settings);
        mConnectedInputDevices[aDeviceName] = ve;
        emit inputDeviceConnected(aDeviceName);
    }
    else
    {
        // try to open an atmega
        Atmega *atmega = new Atmega();
        atmega->openDevice(settings);
        mConnectedInputDevices[aDeviceName] = atmega;
        emit inputDeviceConnected(aDeviceName);
    }

    delete settings;
}


void InputDeviceManager::connectInputDevice(QString /*aDeviceName*/, SerialPortSettings /*aSerialportSetting*/)
{

}


void InputDeviceManager::disconnectInputDevice(QString aDeviceName)
{
    Device *device = getInputDevice(aDeviceName);
    if(!device)
        return;
    device->closeSerialPort();
    mConnectedInputDevices[aDeviceName]->deleteLater();
    mConnectedInputDevices[aDeviceName] = nullptr;
    mConnectedInputDevices.remove(aDeviceName);
    emit inputDeviceDisconnected(aDeviceName);
    mAvailableSerialPorts.push_back(aDeviceName);
    emit inputDeviceAvailable(aDeviceName);
}


Device* InputDeviceManager::getInputDevice(QString aDeviceName)
{
    if(mConnectedInputDevices.contains(aDeviceName))
        return mConnectedInputDevices[aDeviceName];

    return nullptr;
}


QString InputDeviceManager::getDeviceManufacturer(QString aDeviceName)
{
    QSerialPortInfo info(aDeviceName);
    return info.manufacturer();
}


void InputDeviceManager::ignoreSerialPort(QString aDeviceName)
{
    mAvailableSerialPorts.push_back(aDeviceName);
}


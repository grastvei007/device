#include "inputdevicemanager.h"

#include <QSerialPortInfo>
#include <QDebug>

#include "serialportsettings.h"
#include "pidextractdevice.h"
#include "serialdevices/bmw712smart.h"
#include "serialdevices/oscilloscope.h"
#include "serialdevices/atmega.h"


InputDeviceManager& InputDeviceManager::sGetInstance()
{
    static InputDeviceManager sInputDeviceManager;
    return sInputDeviceManager;
}


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
        for(auto i = portNames.begin(); i != portNames.end(); ++i)
        {
            mAvailableSerialPorts.push_back(*i);
            emit inputDeviceAvailable(*i);
        }
        return;
    }

    // test to se if some ports has changed add/remove.
    for(auto i = portNames.begin(); i != portNames.end(); ++i)
    {
        if(!mAvailableSerialPorts.contains(*i))
        {
            mAvailableSerialPorts.push_back(*i);
            emit inputDeviceAvailable(*i);
        }
    }
    QStringList remove;
    for(int i=0; i<mAvailableSerialPorts.size(); ++i)
    {
        if(!portNames.contains(mAvailableSerialPorts.at(i)))
            remove.push_back(mAvailableSerialPorts.at(i));
    }

    for(auto i = remove.begin(); i != remove.end(); ++i)
    {
        mAvailableSerialPorts.removeAll(*i);
        if(mConnectedInputDevices.contains(*i))
        {
            mConnectedInputDevices.remove(*i);
        }
        emit inputDeviceDisconnected(*i);
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

    // try to open an atmega
    Atmega *atmega = new Atmega();
    atmega->openDevice(settings);
    mConnectedInputDevices[aDeviceName] = atmega;
    emit inputDeviceConnected(aDeviceName);


  /*  mPidExtractDevice= new PidExtractDevice();
    connect(mPidExtractDevice, &PidExtractDevice::pidReady, this, &InputDeviceManager::onPidReady);
    connect(mPidExtractDevice, &PidExtractDevice::errorFindingPid, this, &InputDeviceManager::onErrorFindingPid);

    if(!mPidErrorTimer)
        mPidErrorTimer = new QTimer;

   mPidErrorTimer->singleShot(5000, this, &InputDeviceManager::onErrorFindingPid);

    mPidExtractDevice->openDevice(settings);
*/
    delete settings;


}


void InputDeviceManager::connectInputDevice(QString aDeviceName, SerialPortSettings aSerialportSetting)
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


void InputDeviceManager::ignoreSerialPort(QString aDeviceName)
{
    mAvailableSerialPorts.push_back(aDeviceName);
}


void InputDeviceManager::onErrorFindingPid()
{

    mPidExtractDevice->deleteLater();
    mPidExtractDevice = nullptr;
}


void InputDeviceManager::onPidReady()
{
    mPidErrorTimer->deleteLater();
    mPidErrorTimer = nullptr;

    int pid = mPidExtractDevice->getPid();
    QString portName = mPidExtractDevice->getPortName();
    mPidExtractDevice->pidOk();
    mPidExtractDevice->closeSerialPort();
    mPidExtractDevice->deleteLater();
    mPidExtractDevice = nullptr;

    if(!mUseDefaultSerialPortSettingFlag)
        return; //FIXME

    if(pid == 825766721)
    {
        SerialPortSettings setting(mDefaultSerialPortSettings);

        Bmw712Smart *bmv = new Bmw712Smart();
        bmv->openDevice(&setting);
        mConnectedInputDevices[portName] = bmv;
        emit inputDeviceConnected(portName);
    }
    else if(pid == 624832759)
    {
        SerialPortSettings settings(mDefaultSerialPortSettings);
        settings.setName(portName);
        Device *oscilloscope = new Oscilloscope();
        oscilloscope->openDevice(&settings);

    }
    else
    {

    }

    emit inputDeviceConnected(portName);
}

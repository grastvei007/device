#ifndef INPUTDEVICEMANAGER_H
#define INPUTDEVICEMANAGER_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include <QMap>

#include <memory>

#include "device.h"

#include "serialportsettings.h"


class PidExtractDevice;

/**
 * @brief The InputDeviceManager class
 *
 * Manage all serial ports available. Handle new connections and disconnection of devices
 * from the system.
 *
 */
class InputDeviceManager : public QObject
{
    Q_OBJECT
public:
    enum BaudRate{
        e9600,
        e19200,
        e38400,
        e115200
    };

    static InputDeviceManager& sGetInstance();

    void ignoreSerialPort(QString aDeviceName);

    void setDetectInputDevicesInterval(int aIntervalInMs); ///< polling time.

    void setUseDefaultSerialSettingFlag(bool aBool);
    void setDefaultBaudRate(BaudRate aBaudRate);

    int getDefaultBaudRate() const;

    void connectInputDevice(QString aDeviceName); ///< Connect to the input device
    void connectInputDevice(QString aDeviceName, SerialPortSettings aSerialportSetting);
    void disconnectInputDevice(QString aDeviceName); ///< Disconnect an input device, and set it available.

    Device* getInputDevice(QString aDeviceName);
    QString getDeviceManufacturer(QString aDeviceName);
signals:
    void inputDeviceConnected(QString aDeviceName); ///< The name of the input port.
    void inputDeviceDisconnected(QString aDeviceName);
    void inputDeviceAvailable(QString aDeviceName);

private:
    InputDeviceManager();

private slots:
    void detectInputDevices(); ///< polling
    //void onErrorOpenDevice();

private:
    SerialPortSettings mDefaultSerialPortSettings;
    bool mUseDefaultSerialPortSettingFlag;

    QMap<QString, Device*> mConnectedInputDevices; // key portname

    int mPollingIntervalMs;
    std::unique_ptr<QTimer> mPollingIntervalTimer;

    QVector<QString> mAvailableSerialPorts;

    PidExtractDevice *mPidExtractDevice;
    QTimer *mPidErrorTimer;
};

#endif // INPUTDEVICEMANAGER_H

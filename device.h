#ifndef DEVICE_H
#define DEVICE_H

#include "device_global.h"
#include <QSerialPort>

#include <memory>

class QSerialPortInfo;
class SerialPortSettings;

/**
 * @brief The base Device class
 */
class Device : public QObject
{
    Q_OBJECT
public:
    Device();
    virtual ~Device();

    virtual int getPid() const = 0;
    QString getPortName() const;

    void openDevice(SerialPortSettings *aSerialPortSettings);
    void write(const QByteArray &aData); ///< Write data to serial port.

    virtual void dataRead(QByteArray aData); ///< overide to handle data
    void pidOk(); ///< send "pid ok\n" to device.

    void setOverideDataRead(bool aOveride);
    void setDataListenFlag(bool aListen);
    void setDeviceName(QString aName);

    QString getDeviceName() const;
    QString getManufacturer() const;

signals:
    void dataRecieved(QByteArray);
    void deviceAboutToBeDestroyd(Device*);
    void ready(); ///< device is open.
    void deviceClosed();
    void errorOpen();

public slots:
    void closeSerialPort();


protected:
    bool isValid(const QByteArray &aData);

private slots:
    void readData();
    void handleError(QSerialPort::SerialPortError error);
    void onConnectionAboutToClose();
private:
    std::unique_ptr<QSerialPort> mSerialPort;
    std::unique_ptr<QSerialPortInfo> mSerialPortInfo;
    bool mOverideDataReadFlag;
    bool mDataListenFlag;

    QString mName;
    bool mWriteErrorMsg;

};

#endif // DEVICE_H

#ifndef DEVICE_H
#define DEVICE_H

#include "device_global.h"

#include <QSerialPort>

#include <memory>

class QSerialPort;
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

    virtual void dataRead(QByteArray aData); ///< overide to handle data
    void pidOk(); ///< send "pid ok\n" to device.
    void setOverideDataRead(bool aOveride);
    void setDeviceName(QString aName);

    QString getDeviceName() const;

signals:
    void dataRecieved(QByteArray);
    void deviceAboutToBeDestroyd(Device*);

public slots:
    void closeSerialPort();

signals:
    void ready(); ///< device is open.
    void errorOpen();

protected:
    bool isValid(const QByteArray &aData);

private slots:
    void readData();
    void handleError(QSerialPort::SerialPortError error);
private:
    std::unique_ptr<QSerialPort> mSerialPort;
    bool mOverideDataReadFlag;

    QString mName;

};

#endif // DEVICE_H

#ifndef SERIALPORTSETTINGS_H
#define SERIALPORTSETTINGS_H

#include <QtSerialPort/QSerialPort>

class SerialPortSettings
{
public:
    SerialPortSettings();
    SerialPortSettings(const SerialPortSettings &aSerialPortSettings);

    void setName(const QString& aName);
    void setBaudRate(qint32 aBaudRate);
    void setStringBaudRate(const QString& aStringBaudRate);
    void setDataBits(QSerialPort::DataBits aDataBits);
    void setStringDataBits(const QString &aStringDataBits);
    void setParity(QSerialPort::Parity aParity);
    void setStringParity(const QString &aStringParity);
    void setStopBits(QSerialPort::StopBits aStopBits);
    void setStringStopBits(const QString &aStringStopBits);
    void setFlowControl(QSerialPort::FlowControl aFlowControl);
    void setStringFlowControl(const QString& aStringFlowControl);
    void setLocalEchoEnabled(bool aLocalEchoEnabled);

    // getters
    const QString& getName() const;
    qint32 getBaudRate() const;
    const QString& getStringBaudRate() const;
    QSerialPort::DataBits getDataBits() const;
    const QString& getStringDataBits() const;
    QSerialPort::Parity getParity() const;
    const QString& getStringParity() const;
    QSerialPort::StopBits getStopBits() const;
    const QString& getStringStopBits() const;
    QSerialPort::FlowControl getFlowControl() const;
    const QString& stringFlowControl() const;
    bool getLocalEchoEnabled() const;

private:
    QString mName;
    qint32 mBaudRate;
    QString mStringBaudRate;
    QSerialPort::DataBits mDataBits;
    QString mStringDataBits;
    QSerialPort::Parity mParity;
    QString mStringParity;
    QSerialPort::StopBits mStopBits;
    QString mStringStopBits;
    QSerialPort::FlowControl mFlowControl;
    QString mStringFlowControl;
    bool mLocalEchoEnabled;
};

#endif // SERIALPORTSETTINGS_H

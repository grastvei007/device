#include "device.h"

#include <QDebug>
#include <QMessageBox>
#include <QSerialPort>

#include "serialportsettings.h"

Device::Device() :
    mOverideDataReadFlag(false),
    mDataListenFlag(false)
{

    mSerialPort.reset(new QSerialPort);
    connect(mSerialPort.get(), static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error), this, &Device::handleError);
    connect(mSerialPort.get(), static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::errorOccurred), this, &Device::handleError);

    connect(mSerialPort.get(), &QSerialPort::aboutToClose, this, &Device::onConnectionAboutToClose);

    connect(mSerialPort.get(), &QSerialPort::readyRead, this, &Device::readData);
}


Device::~Device()
{
    emit deviceAboutToBeDestroyd(this);
    if(mSerialPort->isOpen())
        closeSerialPort();
}

void Device::dataRead(QByteArray aData)
{
    qDebug() << __FUNCTION__ << "Override function to hande data read from device";

}


void Device::readData()
{
    QByteArray data = mSerialPort->readAll();
    if(mOverideDataReadFlag)
        emit dataRecieved(data);
    else if(mDataListenFlag)
    {
        emit dataRecieved(data);
        dataRead(data);
    }
    else
        dataRead(data);
}

/**
 * @brief Device::setOverideDataRead
 * @param aOveride
 *
 * Set the override flag. This enable the device to emit its recieved data instead of
 * passing it to the dataRead function.
 */
void Device::setOverideDataRead(bool aOveride)
{
    mOverideDataReadFlag = aOveride;
}


void Device::setDataListenFlag(bool aListen)
{
    mDataListenFlag = aListen;
}

void Device::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(nullptr, tr("Critical Error"), mSerialPort->errorString());
        closeSerialPort();
    }
}

void Device::closeSerialPort()
{
    mSerialPort->close();
}


void Device::openDevice(SerialPortSettings *aSerialPortSettings)
{
    mSerialPort->setPortName(aSerialPortSettings->getName());
    mSerialPort->setBaudRate(aSerialPortSettings->getBaudRate());
    mSerialPort->setDataBits(aSerialPortSettings->getDataBits());
    mSerialPort->setParity(aSerialPortSettings->getParity());
    mSerialPort->setStopBits(aSerialPortSettings->getStopBits());
    mSerialPort->setFlowControl(aSerialPortSettings->getFlowControl());
    if (mSerialPort->open(QIODevice::ReadWrite)) {
        qDebug() << __FUNCTION__;
        qDebug() << "Baud: " << mSerialPort->baudRate();
        emit ready();
    } else {
        QMessageBox::information(nullptr, tr("Error"), mSerialPort->errorString());
        emit errorOpen();
    }
}


void Device::write(const QByteArray &aData)
{
    qDebug() << __FUNCTION__ << aData;
    mSerialPort->write(aData);
    //if(!mSerialPort->putChar('c'))
     //   qDebug() << "Error writing to device";
    //mSerialPort->flush();
}


QString Device::getPortName() const
{
    return mSerialPort->portName();
}


bool Device::isValid(const QByteArray &aData)
{
    int sum = 0;
    for(int i=0; i<aData.size(); ++i)
        sum += (int)aData.at(i);

    if(sum == 0)
        return false;

    int r = 256 % sum;
    qDebug() << "R: " << r;
    if (r == 256)
        return true;

    return false;
}

void Device::pidOk()
{
    if(mSerialPort->isOpen())
        mSerialPort->write(QByteArray("pid ok\n"));
}


void Device::setDeviceName(QString aName)
{
    mName = aName;
}

QString Device::getDeviceName() const
{
    if(mName.isEmpty())
        return getPortName();

    return mName;
}


void Device::onConnectionAboutToClose()
{
    qDebug() << __FUNCTION__;
}

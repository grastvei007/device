#include "messagehandler.h"

#include "device.h"


MessageHandler::MessageHandler(Device *aDevice) :
    mDevice(aDevice)
{
    connect(mDevice, &Device::dataRecieved, this, &MessageHandler::onDeviceData);
    mDevice->setOverideDataRead(true);
}


void MessageHandler::onDeviceData(QByteArray aData)
{

    mDataBuffer.append(aData);
    parseData();
}



void MessageHandler::parseData()
{
    int idx = mDataBuffer.indexOf("<msg");
    if(idx < 0)
        return;

    if(mDataBuffer.size() < idx+8)
        return;

    QString msgSize(mDataBuffer.mid(idx+4, 4));
    int size = msgSize.toInt();

    if(mDataBuffer.size() < idx+size)
        return;
}

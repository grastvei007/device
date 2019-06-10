#include "messagehandlermanager.h"

MessageHandlerManager& MessageHandlerManager::sGetInstance()
{
    static MessageHandlerManager sMessageHandlerManager;
    return sMessageHandlerManager;
}

MessageHandlerManager::MessageHandlerManager()
{

}

void MessageHandlerManager::createMessageHandlerForDevice(Device *aDevice)
{
    MessageHandler *handler = new MessageHandler(aDevice);
    mMessageHandlers[aDevice] = handler;
    connect(aDevice, &Device::deviceAboutToBeDestroyd, this, &MessageHandlerManager::onDeviceDestroyed);
}


MessageHandler* MessageHandlerManager::getMessageHandlerForDevice(Device *aDevice) const
{
    return mMessageHandlers[aDevice];
}


void MessageHandlerManager::onDeviceDestroyed(Device *aDevice)
{
    if(!mMessageHandlers.contains(aDevice))
        return;

    MessageHandler *handler = mMessageHandlers[aDevice];
    if(handler)
    {
        handler->deleteLater();
        mMessageHandlers.remove(aDevice);
    }

}

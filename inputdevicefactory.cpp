#include "inputdevicefactory.h"

InputDeviceFactory& InputDeviceFactory::sInstance()
{
    static InputDeviceFactory sInputDeviceFactory;
    return sInputDeviceFactory;
}

InputDeviceFactory::InputDeviceFactory()
{

}


Device* InputDeviceFactory::create(int /*aPid*/)
{
    return nullptr;
}


#ifndef INPUTDEVICEFACTORY_H
#define INPUTDEVICEFACTORY_H

#include <QMap>

#include "device.h"

class InputDeviceFactory
{
public:
     static InputDeviceFactory& sInstance();

    //template<class T>
    //void createFactory(int aPid);

    Device* create(int aPid);


private:
    InputDeviceFactory();

private:
    QMap<QString, QString> mInputDeviceByPid;

};

#endif // INPUTDEVICEFACTORY_H

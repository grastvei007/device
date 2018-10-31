#ifndef PIDEXTRACTDEVICE_H
#define PIDEXTRACTDEVICE_H

#include "device.h"

class PidExtractDevice : public Device
{
    Q_OBJECT
public:
    PidExtractDevice();

    int getPid() const override;
    void dataRead(QByteArray aData) override;

signals:
    void pidReady();
    void errorFindingPid();

private:
    int mPid;
    int mPidCounter;
};

#endif // PIDEXTRACTDEVICE_H

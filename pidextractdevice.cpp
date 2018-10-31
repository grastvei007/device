#include "pidextractdevice.h"

#include <QStringList>
#include <QDebug>

PidExtractDevice::PidExtractDevice() :
    mPid(-1),
    mPidCounter(10)
{

}


int PidExtractDevice::getPid() const
{
    return mPid;
}


void PidExtractDevice::dataRead(QByteArray aData)
{
    if(mPidCounter < 0)
    {
        emit errorFindingPid();
        return;
    }

    QStringList messages = QString(aData).split("\n");

    for(int i=0; i<messages.size(); ++i)
    {
        if(messages[i].contains("PID"))
        {
            QString pidstr = messages[i].split("\t").last();
            qDebug() << "Pid str " << pidstr;
            union
            {
                int intPid;
                char bytePid[4];
            }pid;

            QByteArray b = pidstr.toLocal8Bit();

            pid.bytePid[0] = b[2];
            pid.bytePid[1] = b[3];
            pid.bytePid[2] = b[4];
            pid.bytePid[3] = b[5];

            mPid = pid.intPid;
            qDebug() << "pid int: " << mPid;
            mPidCounter = -1; // done parsing pid found.
            emit pidReady();

        }
    }


    mPidCounter--;


}

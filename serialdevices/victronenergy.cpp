#include "victronenergy.h"

#include <QDebug>

#include <tagsystem/taglist.h>

VictronEnergy::VictronEnergy() :
    mChecksum(0),
    mState(eIdle)
{

}


int VictronEnergy::getPid() const
{
    return 12;
}


void VictronEnergy::dataRead(QByteArray aData)
{
    //qDebug() << aData;

    for(int i=0; i<aData.size(); ++i)
        recordFrame(aData[i]);
}


void VictronEnergy::recordFrame(char c)
{
    mChecksum += c;

    c = toupper(c);

    switch (mState)
    {
    case eIdle:
        {
        switch (c)
            {
            case '\n':
                mName.clear();
                mValue.clear();
                mReadFrame.clear();
                mState = eName;
                break;
            case '\r':
            default:
                break;

            }
        break;
    }
    case eBegin:
        break;
    case eName:
    {
        switch(c)
        {
            case '\t':
                if(mName.contains("Checksum", Qt::CaseInsensitive))
                {
                    mState = eChecksum;
                    break;
                }

                mState = eValue;
                break;

        default:
            mName.append(c);
            break;
        }

        break;
    }
    case eValue:
    {
        switch (c)
        {
            case '\n':
                mReadFrame[mName] = mValue;
                mState = eName;
                break;

            case 'r':
                break;
        default:
            mValue.append(c);
            break;
        }
        break;
    }
    case eChecksum:

        int s = mChecksum % 256;
        if(s != 0)
            qDebug() << "Invalid frame, check: " << s;
        else
        {
            if(mReadFrame.contains("PID") && getDeviceName().isEmpty())
            {
                QString pid = mReadFrame["PID"];
                setDeviceName(pidToDeviceName(pid));
            }
            if(!getDeviceName().isEmpty())
                putValuesOnTags();
        }
        mChecksum = 0;
        mState = eIdle;
        break;

    }
}


void VictronEnergy::putValuesOnTags()
{
    for(auto iter = mReadFrame.begin(); iter != mReadFrame.end(); ++iter)
    {
        if(iter.key() == "PID")
            continue;
        if(mTagsockets.contains(iter.key()))
        {

        }
        else
        {

        }
    }
}

QString VictronEnergy::pidToDeviceName(const QString &aPid)
{
    if(aPid.contains("0XA381"))
        qDebug() << "BMV712 Smart";
    return QString();
}

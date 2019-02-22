#include "oscilloscope.h"

#include <QTimer>

#include <tagsystem/taglist.h>
#include <tagsystem/tag.h>
#include <tagsystem/tagsocket.h>



Oscilloscope::Oscilloscope() : Device()
{
    mUpdateHz = TagList::sGetInstance().createTag("oscilloscope", "hz", Tag::eInt);
    mReading = TagList::sGetInstance().createTag("oscilloscope", "data", Tag::eDouble);

    mUpdateHz->setValue(10);

    mUpdateHzTagSocket = TagSocket::createTagSocket("oscilloscope", "hz", TagSocket::eInt);
    mUpdateHzTagSocket->hookupTag(mUpdateHz);
    connect(mUpdateHzTagSocket, static_cast<void(TagSocket::*)(int)>(&TagSocket::valueChanged), this, &Oscilloscope::onUpdateHzTagSocketValueChanged);

    mTimer = new QTimer(this);
    mTimer->setInterval(100);
    connect(mTimer, &QTimer::timeout, this, &Oscilloscope::putFrameOnTag);
}


Oscilloscope::~Oscilloscope()
{

}

void Oscilloscope::dataRead(QByteArray aData)
{
    mBuffer.push_back(aData);
}

void Oscilloscope::onUpdateHzTagSocketValueChanged(int aValue)
{

}


void Oscilloscope::putFrameOnTag()
{

}

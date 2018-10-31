#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include <QObject>
#include "device.h"

class Tag;
class TagSocket;
class QTimer;

class Oscilloscope : public Device
{
    Q_OBJECT
public:
    Oscilloscope();
    virtual ~Oscilloscope();

    int getPid() const {return 624832759;}

    void dataRead(QByteArray aData); ///< parse input data.
signals:

private slots:
    void onUpdateHzTagSocketValueChanged(int aValue);
    void putFrameOnTag();

private:
    QByteArray mBuffer;

    Tag *mUpdateHz;
    Tag *mReading;

    TagSocket *mUpdateHzTagSocket;
    QTimer *mTimer;
};

#endif // OSCILLOSCOPE_H

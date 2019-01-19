#include "messagereader.h"

MessageReader::MessageReader(QByteArray *aMessage) :
    mMessage(aMessage)
{

}

/**
 * @brief MessageReader::isValid
 *
 * Validated an array containing a message.
 *
 * @param aMsg
 * @return true/false.
 */
bool MessageReader::isValid(QByteArray &aMsg, QString &rError)
{
    if(!aMsg.startsWith("<msg"))
    {
        rError.append("Message does not start with, <msg\n");
        return false;
    }
    if(!aMsg.endsWith(">"))
    {
        rError.append("Message does not end with, >\n");
        return false;
    }
    if(aMsg.size() < 10)
    {
        rError.append("Message size is to small\n");
        return false;
    }


    QString size(aMsg.mid(4,4));
    int n = size.toInt();
    if( aMsg.size() != n)
    {
        rError.append("Message size is wrong\n");
        return false;
    }
    int sum = 0;
    for(int i=0; i<n; ++i)
    {
        int c = (int)aMsg.at(i);
        // just make sure it is in range.
        while(c < 0)
            c += 255;
        while(c > 255)
            c -= 255;

        sum += c;
    }

    int r = sum % 255;
    if(aMsg.at(n-1) != r)
    {
        rError.append("Message checksum is wrong");
        return false;
    }

    return true;
}


void MessageReader::parse()
{
    QString key;
    bool hasKey = false;
    for(int i=8; i<mMessage->size(); ++i)
    {
        if(mMessage->at(i) == ':')
        {
            hasKey = true;
            continue;
        }

        if(!hasKey)
            key.append(mMessage->at(i));
        else
        {
            if(mMessage->at(i) == 'c')
            {
                union U
                {
                    short s;
                    char byte[2];
                }u;
                u.byte[0] = mMessage->at(++i);
                u.byte[1] = mMessage->at(++i);
                QByteArray val = mMessage->mid(i, u.s);
                i += u.s;
                MessagePair *pair = new MessagePair;
                pair->setKey(key);
                pair->setStringFromBytess(val.data(), u.s);
                mMessagePairs.push_back(pair);
                hasKey = false;
                key.clear();
            }
            else if(mMessage->at(i) == 'f')
            {

            }
            else if(mMessage->at(i) == 'i')
            {

            }
            else if(mMessage->at(i) == 'b')
            {

            }
        }
    }
}


int MessageReader::getNumberOfPairs() const
{

}


MessagePair* MessageReader::getMessagePairByIndex(int aIdx) const
{

}

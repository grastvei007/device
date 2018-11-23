#include "message.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Message::Message()
{
    mMessageSize = 8;
    mMessage = new unsigned char[mMessageSize];
    mMessage[0] = '<';
    mMessage[1] = 'm';
    mMessage[2] = 's';
    mMessage[3] = 'g';
    mMessage[4] = '0';
    mMessage[5] = '0';
    mMessage[6] = '0';
    mMessage[7] = '0'; // with place for 4 byte int value in plain text.

}

void Message::add(char* aKey, double aValue)
{

}


void Message::add(char *aKey, float aValue)
{
    size_t keySize = strlen(aKey);
    char *pair = new char[keySize+6];

    int t=0;
    for(int i=0; i<keySize; ++i)
    {
        pair[i] = aKey[i];
        t++;
    }
    pair[t++] = ':';
    pair[t++] = 'f'; // mark value as float.

    unsigned char bytes[4];
    memcpy(bytes, (unsigned char*) &aValue, 4);
    pair[t++] = bytes[0];
    pair[t++] = bytes[1];
    pair[t++] = bytes[2];
    pair[t++] = bytes[3];

    unsigned char *temp = new unsigned char[mMessageSize+keySize+6];
    memcpy(temp, mMessage, mMessageSize);
    int k=0;
    for(int i=mMessageSize; i<(mMessageSize+keySize+6); ++i)
        temp[i] = pair[k++];
    mMessageSize += (keySize+6);
    delete [] mMessage;
    mMessage = temp;
    temp = nullptr;
}


void Message::add(char* aKey, int aValue)
{

}


void Message::finnish()
{
    unsigned char *temp = new unsigned char[mMessageSize+2];
    memcpy(temp, mMessage, mMessageSize);
    temp[mMessageSize] = '0';
    temp[mMessageSize+1] = '>';
    delete [] mMessage;
    mMessage = temp;
    temp = nullptr;
    mMessageSize += 2;

    char buffer[4] = {0};
    sprintf(buffer, "%i", (int)mMessageSize);
    int t = strlen(buffer);
    if(t == 1)
         mMessage[7] = buffer[0];
    else if(t == 2)
    {
        mMessage[6] = buffer[0];
        mMessage[7] = buffer[1];
    }
    else if( t == 3)
    {
        mMessage[5] = buffer[0];
        mMessage[6] = buffer[1];
        mMessage[7] = buffer[2];
    }
    else if(t == 4)
    {
        for(int i=0; i<t; ++i)
            mMessage[5+i] = buffer[i];
    }
    calcCheckcode();
}


int Message::getMessage(unsigned char *&rMessage)
{
    rMessage = new unsigned char[mMessageSize];
    memcpy(rMessage, mMessage, mMessageSize);
    return mMessageSize;
}


void Message::calcCheckcode()
{
    int n = 0;
    for(int i=0; i<mMessageSize; ++i)
    {
        if(i != mMessageSize -2)
            n += (int)mMessage[i];
    }

    mMessage[mMessageSize-2] = (char)(n % 256);
}

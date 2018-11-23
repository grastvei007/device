#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdio.h>

class Message
{
public:
    Message();

    void add(char* aKey, double aValue);
    void add(char *aKey, float aValue);
    void add(char* aKey, int aValue);

    void finnish(); ///< prepare meassege to be transmited.

    int getMessage(unsigned char *&rMessage); ///< get the message
private:
    void calcCheckcode();

private:
    unsigned char *mMessage;
    size_t mMessageSize;

};

#endif // MESSAGE_H

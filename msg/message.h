#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdio.h>

/**
 * @brief The Message class
 *
 *
 * The message starts with <msg and ends with r> where r is a control byte.
 * the sum of all bytes in the message modulo 256 = r.
 * after <msg is 4 bytes that describes the size of the message, it is an int in
 * plain text.
 *
 */
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

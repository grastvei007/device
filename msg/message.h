#ifndef MESSAGE_H
#define MESSAGE_H

#include <QByteArray>


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
    Message(const QByteArray &aMsg);

    /**
     * @brief isValid
     *
     * error codes:
     *   1 - all good
     *  -1 - error length
     *  -2 - error in header
     *  -3 - checksum error
     *
     *
     * @return int an error code
     */
    int isValid() const;

    void add(const QString &aKey, double aValue);
    void add(const QString &Key, float aValue);
    void add(const QString &aKey, int aValue);
    void add(const QString &aKey, bool aValue);
    void add(const QString &aKey, QString aValue);

    void finnish(); ///< prepare meassege to be transmited.

    QByteArray getMessage() const; ///< get the message

    QString getAtmegaDeviceName();
private:
    void calcCheckcode();
    int validateMessage(const QByteArray &aMsg);

private:
    QByteArray mMessage;
    int mIsValid;

};

#endif // MESSAGE_H

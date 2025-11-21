#include <gtest/gtest.h>

#include <QString>
#include <msg/message.h>

class TestMessge : public ::testing::Test
{
protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(TestMessge, message_header)
{
    Message message;

    EXPECT_EQ(message.getMessage(), QByteArray("<msg00"));
}

TEST_F(TestMessge, empty_message)
{
    Message message;
    message.finnish();

    QByteArray msg = message.getMessage();
    EXPECT_EQ(msg.size(), 8);

    EXPECT_TRUE(message.isValid());
}

TEST_F(TestMessge, request_device_name)
{
    Message msg;
    msg.add(QString("deviceName"), QString(""));
    msg.finnish();

    EXPECT_TRUE(msg.isValid());
}

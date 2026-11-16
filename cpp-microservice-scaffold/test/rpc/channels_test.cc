#include <gtest/gtest.h>
#include "rpc.h"

// 1. 空节点集合测试
TEST(ChannelsTest, Empty)
{
    biterpc::Channels channels;

    // 没有节点时，不能返回有效 Channel
    EXPECT_EQ(channels.select(), nullptr);

    // 没有节点时，不能返回地址
    EXPECT_FALSE(
        channels.selectAddr().has_value()
    );
}

// 2. 轮询选择测试
TEST(ChannelsTest, RoundRobin)
{
    biterpc::Channels channels;

    channels.insert("127.0.0.1:19001");
    channels.insert("127.0.0.1:19002");

    auto addr1 = channels.selectAddr();
    auto addr2 = channels.selectAddr();
    auto addr3 = channels.selectAddr();

    ASSERT_TRUE(addr1.has_value());
    ASSERT_TRUE(addr2.has_value());
    ASSERT_TRUE(addr3.has_value());

    EXPECT_EQ(addr1.value(), "127.0.0.1:19001");
    EXPECT_EQ(addr2.value(), "127.0.0.1:19002");
    EXPECT_EQ(addr3.value(), "127.0.0.1:19001");
}

// 3. 获取 Channel 测试
TEST(ChannelsTest, SelectChannel)
{
    biterpc::Channels channels;

    channels.insert("127.0.0.1:19001");

    auto channel = channels.select();

    EXPECT_NE(channel, nullptr);
}

// 4. 节点移除测试
TEST(ChannelsTest, Remove)
{
    biterpc::Channels channels;

    channels.insert("127.0.0.1:19001");
    channels.insert("127.0.0.1:19002");

    channels.remove("127.0.0.1:19001");

    auto addr = channels.selectAddr();

    ASSERT_TRUE(addr.has_value());

    EXPECT_EQ(addr.value(), "127.0.0.1:19002");

    channels.remove("127.0.0.1:19002");

    EXPECT_EQ(channels.select(), nullptr);
    EXPECT_FALSE(channels.selectAddr().has_value());
}
TEST(ChannelsTest, RemoveDuringRoundRobin)
{
    biterpc::Channels channels;

    channels.insert("127.0.0.1:19001");
    channels.insert("127.0.0.1:19002");
    channels.insert("127.0.0.1:19003");

    // 连续选择两次，此时 _idx 应为 2
    channels.selectAddr();
    channels.selectAddr();

    // 删除一个节点，容器大小变为 2
    channels.remove("127.0.0.1:19002");

    // 此时应正确取模，不能直接访问 _channels[2]
    auto addr = channels.selectAddr();

    ASSERT_TRUE(addr.has_value());
    EXPECT_EQ(addr.value(), "127.0.0.1:19001");

    // 再删除一个节点
    channels.remove("127.0.0.1:19003");

    // 只剩 9001，后续所有选择都应返回它
    for (int i = 0; i < 5; i++)
    {
        auto result = channels.selectAddr();

        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), "127.0.0.1:19001");
    }
}
int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
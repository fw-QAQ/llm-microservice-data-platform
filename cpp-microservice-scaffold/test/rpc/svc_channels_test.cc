#include <gtest/gtest.h>

#include "rpc.h"

// 1. 未注册服务
TEST(SvcChannelsTest, UnwatchedService)
{
    biterpc::SvcChannels scs;

    // 没有注册 user
    EXPECT_EQ(scs.getNode("user"), nullptr);

    EXPECT_FALSE(
        scs.getNodeAddr("user").has_value()
    );

    // 直接添加节点，不应该自动注册服务
    scs.addNode("user", "127.0.0.1:19001");

    EXPECT_EQ(scs.getNode("user"), nullptr);
}


// 2. 注册服务但未添加节点
TEST(SvcChannelsTest, WatchWithoutNode)
{
    biterpc::SvcChannels scs;

    scs.setWatch("user");

    // 服务已注册，但节点集合仍为空
    EXPECT_EQ(scs.getNode("user"), nullptr);

    EXPECT_FALSE(
        scs.getNodeAddr("user").has_value()
    );
}


// 3. 添加节点并轮询
TEST(SvcChannelsTest, AddAndRoundRobin)
{
    biterpc::SvcChannels scs;

    scs.setWatch("user");

    scs.addNode("user", "127.0.0.1:19001");
    scs.addNode("user", "127.0.0.1:19002");

    auto addr1 = scs.getNodeAddr("user");
    auto addr2 = scs.getNodeAddr("user");
    auto addr3 = scs.getNodeAddr("user");

    ASSERT_TRUE(addr1.has_value());
    ASSERT_TRUE(addr2.has_value());
    ASSERT_TRUE(addr3.has_value());

    EXPECT_EQ(addr1.value(), "127.0.0.1:19001");
    EXPECT_EQ(addr2.value(), "127.0.0.1:19002");
    EXPECT_EQ(addr3.value(), "127.0.0.1:19001");

    // 验证能够获取 Channel
    EXPECT_NE(scs.getNode("user"), nullptr);
}


// 4. 多个服务彼此独立
TEST(SvcChannelsTest, MultipleServices)
{
    biterpc::SvcChannels scs;

    scs.setWatch("user");
    scs.setWatch("message");

    scs.addNode("user", "127.0.0.1:19001");
    scs.addNode("user", "127.0.0.1:19002");

    scs.addNode("message", "127.0.0.1:19101");

    auto user1 = scs.getNodeAddr("user");
    auto message1 = scs.getNodeAddr("message");
    auto user2 = scs.getNodeAddr("user");

    ASSERT_TRUE(user1.has_value());
    ASSERT_TRUE(message1.has_value());
    ASSERT_TRUE(user2.has_value());

    EXPECT_EQ(user1.value(), "127.0.0.1:19001");
    EXPECT_EQ(message1.value(), "127.0.0.1:19101");
    EXPECT_EQ(user2.value(), "127.0.0.1:19002");
}


// 5. 删除节点并重复注册服务
TEST(SvcChannelsTest, RemoveAndRepeatWatch)
{
    biterpc::SvcChannels scs;

    scs.setWatch("user");

    scs.addNode("user", "127.0.0.1:19001");
    scs.addNode("user", "127.0.0.1:19002");

    // 再次注册同一个服务
    // 不应该清除已有节点
    scs.setWatch("user");

    EXPECT_NE(scs.getNode("user"), nullptr);

    // 删除第一个节点
    scs.delNode("user", "127.0.0.1:19001");

    auto addr = scs.getNodeAddr("user");

    ASSERT_TRUE(addr.has_value());
    EXPECT_EQ(addr.value(), "127.0.0.1:19002");

    // 删除剩下的节点
    scs.delNode("user", "127.0.0.1:19002");

    EXPECT_EQ(scs.getNode("user"), nullptr);

    EXPECT_FALSE(
        scs.getNodeAddr("user").has_value()
    );
}
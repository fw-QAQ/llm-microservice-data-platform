#include <gtest/gtest.h>
#include "json_util.h"

TEST(JsonUtilTest, SerializeAndUnserialize)
{
    Json::Value value;

    value["name"] = "John Doe";
    value["age"] = 30;
    value["is_student"] = false;

    value["skills"].append("C++");
    value["skills"].append("Linux");

    // 序列化
    std::string str;

    ASSERT_TRUE(bitejson::serialize(value, &str));
    ASSERT_FALSE(str.empty());

    // 反序列化
    Json::Value result;
    std::string err;

    ASSERT_TRUE(bitejson::unserialize(str, &result, &err));

    // 验证数据
    EXPECT_EQ(result["name"].asString(), "John Doe");
    EXPECT_EQ(result["age"].asInt(), 30);
    EXPECT_FALSE(result["is_student"].asBool());

    ASSERT_TRUE(result["skills"].isArray());
    ASSERT_EQ(result["skills"].size(), 2);

    EXPECT_EQ(result["skills"][0].asString(), "C++");
    EXPECT_EQ(result["skills"][1].asString(), "Linux");
}

TEST(JsonUtilTest, InvalidJson)
{
    std::string str = R"({"name":"Tom" "age":20})";

    Json::Value value;
    std::string err;

    EXPECT_FALSE(bitejson::unserialize(str, &value, &err));
    EXPECT_FALSE(err.empty());
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
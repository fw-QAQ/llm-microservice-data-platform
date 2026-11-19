#pragma once

#include <string>
#include <optional>
#include <memory>
#include <cstddef>

// 前向声明，避免在公开头文件中包含 Redis++ SDK
namespace sw::redis
{
    class Redis;
}

namespace biteredis
{

struct redis_settings
{
    std::string host = "redis-service";
    int port = 6379;
    std::string password;

    int db = 0;

    // 单位：毫秒
    int connect_timeout = 1500;
    int socket_timeout = 2000;

    std::size_t pool_size = 3;
    int pool_wait_timeout = 1500;
};

class RedisClient
{
public:
    explicit RedisClient(const redis_settings& settings);

    ~RedisClient();

    // 禁止复制客户端，避免产生不明确的所有权
    RedisClient(const RedisClient&) = delete;
    RedisClient& operator=(const RedisClient&) = delete;

    // 测试 Redis 是否可以正常通信
    bool ping();

    // String 操作
    bool set(
        const std::string& key,
        const std::string& value);

    std::optional<std::string> get(
        const std::string& key);

    long long del(
        const std::string& key);

    long long incr(
        const std::string& key);
    // 写入键值对，并设置过期时间
    bool setex(
        const std::string& key,
        const std::string& value,
        long long seconds);

// 仅当 Key 不存在时写入，同时设置过期时间
    bool setnx(
        const std::string& key,
        const std::string& value,
        long long seconds);
private:
    std::unique_ptr<sw::redis::Redis> redis_;
};

} // namespace biteredis
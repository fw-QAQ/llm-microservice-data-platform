#include "redis.h"

#include <chrono>
#include <memory>
#include <sw/redis++/redis++.h>
#include <stdexcept>
namespace biteredis
{

RedisClient::RedisClient(
    const redis_settings& settings)
{
    if(settings.pool_size == 0
    || settings.port <= 0
    || settings.connect_timeout < 0
    || settings.socket_timeout < 0)
    {
        throw std::invalid_argument("settings are invalid");
    }
    sw::redis::ConnectionOptions options;
    options.host = settings.host;
    options.port = settings.port;
    options.password = settings.password;
    options.db = settings.db;
    options.connect_timeout = std::chrono::milliseconds(settings.connect_timeout);
    options.socket_timeout = std::chrono::milliseconds(settings.socket_timeout);
    sw::redis::ConnectionPoolOptions pool_opts;
    pool_opts.wait_timeout = std::chrono::milliseconds(settings.pool_wait_timeout);
    pool_opts.size = settings.pool_size;
    redis_ = std::make_unique<sw::redis::Redis>(options,pool_opts);

}

RedisClient::~RedisClient() = default;

bool RedisClient::ping()
{

    std::string ret = redis_->ping();
    if(ret  != "PONG")
    {
        return false;
    }
    return true;
}



bool RedisClient::set(
    const std::string& key,
    const std::string& value)
{

    return redis_->set(key, value);
}

std::optional<std::string> RedisClient::get(
    const std::string& key)
{
    auto value = redis_->get(key);
    if(value)
    {
        return value.value();
    }
    return std::nullopt;
}

long long RedisClient::del(
    const std::string& key)
{
    return redis_->del(key);
}

long long RedisClient::incr(
    const std::string& key)
{
    return redis_->incr(key);
}

bool RedisClient::setex(
    const std::string& key,
    const std::string& value,
    long long seconds)
{

    if(seconds <= 0)
    {
        throw std::invalid_argument("seconds must be greater than 0");
    }
    return redis_->set(key, value, std::chrono::seconds(seconds));

}

bool RedisClient::setnx(
    const std::string& key,
    const std::string& value,
    long long seconds)
{

    if(seconds <= 0)
    {
        throw std::invalid_argument("seconds must be greater than 0");
    }
    return redis_->set(key,
        value,
        std::chrono::seconds(seconds),
        sw::redis::UpdateType::NOT_EXIST);

}

} // namespace biteredis
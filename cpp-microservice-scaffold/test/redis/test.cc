#include "redis.h"
#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <thread>

int main() {
  const char *password = std::getenv("REDIS_PASSWORD");

  if (password == nullptr || password[0] == '\0') {
    std::cerr << "[Test] 未配置 REDIS_PASSWORD" << std::endl;
    return 1;
  }

  try {
    biteredis::redis_settings settings;

    settings.host = "redis-service";
    settings.port = 6379;
    settings.password = password;
    settings.pool_size = 3;

    biteredis::RedisClient client(settings);

    // 1. PING
    if (!client.ping()) {
      std::cerr << "[Test] PING 失败" << std::endl;
      return 1;
    }

    std::cout << "[Test] PING 成功" << std::endl;

    const std::string key = "lab:scaffold:counter";

    // 清理可能存在的历史测试数据
    client.del(key);

    // 2. SET
    if (!client.set(key, "0")) {
      std::cerr << "[Test] SET 失败" << std::endl;
      return 1;
    }

    std::cout << "[Test] SET 成功" << std::endl;

    // 3. GET
    auto value = client.get(key);

    if (!value || *value != "0") {
      std::cerr << "[Test] GET 失败" << std::endl;
      return 1;
    }

    std::cout << "[Test] GET 成功: " << *value << std::endl;

    // 4. INCR
    long long first = client.incr(key);
    long long second = client.incr(key);

    if (first != 1 || second != 2) {
      std::cerr << "[Test] INCR 失败" << std::endl;
      return 1;
    }

    std::cout << "[Test] INCR 成功: " << second << std::endl;

    // 5. DEL
    long long count = client.del(key);

    if (count != 1) {
      std::cerr << "[Test] DEL 失败" << std::endl;
      return 1;
    }

    std::cout << "[Test] DEL 成功" << std::endl;

    // 6. 验证 Key 已不存在
    value = client.get(key);

    if (value.has_value()) {
      std::cerr << "[Test] 删除验证失败" << std::endl;
      return 1;
    }

    std::cout << "[Test] 删除验证通过" << std::endl;
    // ========== SETNX 测试 ==========
    const std::string key2 = "lab:scaffold:setnx";

    client.del(key2);

    bool nx_first = client.setnx(key2, "first", 30);
    bool nx_second = client.setnx(key2, "second", 30);

    auto value2 = client.get(key2);

    if (!nx_first || nx_second || !value2 || *value2 != "first") {
      std::cerr << "[Test] SETNX 验证失败" << std::endl;
      return 1;
    }

    std::cout << "[Test] SETNX 验证通过" << std::endl;
    client.del(key2);
    {
    const std::string key_ex = "lab:scaffold:setex";

    client.del(key_ex);

    // 设置 2 秒过期时间
    bool ok = client.setex(key_ex, "hello", 2);

    if (!ok || client.get(key_ex) != std::optional<std::string>("hello"))
    {
        std::cerr << "[Test] SETEX 失败" << std::endl;
        return 1;
    }

    std::cout << "[Test] SETEX 成功" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(3));

    if (client.get(key_ex).has_value())
    {
        std::cerr << "[Test] SETEX 过期验证失败" << std::endl;
        return 1;
    }

    std::cout << "[Test] SETEX 过期验证通过" << std::endl;
}
    std::cout << "[Test] RedisClient 所有测试通过" << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "[Test] 异常: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
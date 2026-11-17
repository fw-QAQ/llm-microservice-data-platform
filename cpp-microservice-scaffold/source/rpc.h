#pragma once

#include <brpc/channel.h>

#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <functional>
#include <brpc/callback.h>
#include <brpc/server.h>

namespace biterpc {

// brpc Channel 智能指针
using ChannelPtr = std::shared_ptr<brpc::Channel>;

// 指定服务的信道管理类
class Channels {
public:
  using ptr = std::shared_ptr<Channels>;

  Channels();

  // 1. 新增服务节点
  void insert(const std::string &addr);

  // 2. 删除服务节点
  void remove(const std::string &addr);

  // 3. 轮询获取节点信道
  ChannelPtr select();

  // 4. 轮询获取节点地址
  std::optional<std::string> selectAddr();

private:
  // 保护服务节点集合及轮询计数
  std::mutex _mtx;

  // 预留服务名称，当前阶段暂未使用
  std::string _svc_name;

  // 轮询下标
  uint32_t _idx;

  // 节点地址与 Channel 的对应关系
  std::vector<std::pair<std::string, ChannelPtr>> _channels;
};
// 多服务信道管理类
class SvcChannels
{
public:
    using ptr = std::shared_ptr<SvcChannels>;

    SvcChannels() = default;

    // 注册需要管理的服务
    void setWatch(const std::string &svc_name);

    // 给指定服务添加节点
    void addNode(
        const std::string &svc_name,
        const std::string &addr
    );

    // 删除指定服务的节点
    void delNode(
        const std::string &svc_name,
        const std::string &addr
    );

    // 获取指定服务的 Channel
    ChannelPtr getNode(const std::string &svc_name);

    // 获取指定服务的节点地址
    std::optional<std::string> getNodeAddr(
        const std::string &svc_name
    );

private:
    // 根据服务名获取 Channels 管理对象
    Channels::ptr _Channels(const std::string &svc_name);

private:
    // 保护服务映射表
    std::mutex _mtx;
    // 服务名 → Channels 管理对象
    std::unordered_map<std::string, Channels::ptr> _maps;
};

// 异步 RPC 回调工厂
class ClosureFactory
{
public:
    // 无参数、无返回值的业务回调
    using callback_t = std::function<void()>;

    // 根据业务回调创建 Protobuf Closure
    static google::protobuf::Closure* create(callback_t&& cb);

private:
    // 保存业务回调的对象
    struct Object
    {
        using ptr = std::shared_ptr<Object>;

        callback_t callback;
    };

    // brpc Closure 最终执行的函数
    static void asyncCallback(const Object::ptr obj);
};
// RPC 服务端工厂
class RpcServerFactory
{
public:
    // 创建并启动 RPC Server
    // 约定：Service 必须使用 new 创建
    static std::shared_ptr<brpc::Server> create(
        int port,
        google::protobuf::Service* svc
    );
};

} // namespace biterpc

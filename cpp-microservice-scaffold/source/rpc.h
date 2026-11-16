#pragma once

#include <brpc/channel.h>

#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace biterpc
{

// brpc Channel 智能指针
using ChannelPtr = std::shared_ptr<brpc::Channel>;

// 指定服务的信道管理类
class Channels
{
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
    std::vector<
        std::pair<std::string, ChannelPtr>
    > _channels;
};

} // namespace biterpc

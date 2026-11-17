#pragma once

#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Watcher.hpp>
#include <etcd/Response.hpp>

#include <functional>
#include <memory>
#include <string>

namespace bitesvc
{

// =======================
// 服务注册类
// =======================

class SvcProvider
{
public:
    using ptr = std::shared_ptr<SvcProvider>;

    SvcProvider(
        const std::string &reg_center_addr,
        const std::string &svc_name,
        const std::string &svc_addr
    );

    // 注册服务
    bool registry();

private:
    // 生成服务注册 Key
    std::string make_key();

private:
    std::string _reg_center_addr;
    std::string _instance_id;
    std::string _svc_name;
    std::string _svc_addr;

    std::shared_ptr<etcd::KeepAlive> _keepalive;
};


// =======================
// 服务发现类
// =======================

class SvcWatcher
{
public:
    using ptr = std::shared_ptr<SvcWatcher>;

    // 服务状态变化回调的类型别名
    using ModCallbck =
        std::function<void(std::string, std::string)>;

    SvcWatcher(
        const std::string &reg_center_addr,
        ModCallbck &&online_callback,
        ModCallbck &&offline_callback
    );

    // 启动服务发现
    bool watch();

private:
    // 处理 Watch 事件
    void callback(const etcd::Response &resp);

    // 从 Key 中解析服务名称
    std::string parse_key(const std::string &key);

private:
    std::string _reg_center_addr;

    ModCallbck _online_callback;
    ModCallbck _offline_callback;

    std::shared_ptr<etcd::Watcher> _watcher;
};

} // namespace bitesvc

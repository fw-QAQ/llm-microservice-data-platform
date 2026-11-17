#include "etcd.h"
#include "util.h"

#include <chrono>
#include <cstdint>
#include <etcd/Watcher.hpp>
#include <iostream>
#include <random>
#include <sstream>
#include <thread>
#include <utility>

namespace bitesvc
{

namespace
{

// 用标准库生成实验用实例标识
std::string generate_instance_id()
{
    std::random_device rd;

    std::mt19937_64 engine(rd());

    std::stringstream ss;

    ss << std::hex
       << engine()
       << engine();

    return ss.str();
}

// 等待 Etcd 连接成功
void wait_for_connection(etcd::Client &client)
{
    while (!client.head().get().is_ok())
    {
        std::cerr
            << "[Etcd] 连接服务器失败，准备重试"
            << std::endl;

        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
    }
}

} // anonymous namespace


// SvcProvider 构造函数
SvcProvider::SvcProvider(
    const std::string& reg_center_addr,
    const std::string& svc_name,
    const std::string& svc_addr)
    : _reg_center_addr(reg_center_addr),
      _instance_id(biteutil::Random::code()),
      _svc_name(svc_name),
      _svc_addr(svc_addr)
{
}


// SvcWatcher 构造函数
SvcWatcher::SvcWatcher(
    const std::string &reg_center_addr,
    ModCallbck &&online_callback,
    ModCallbck &&offline_callback)
    : _reg_center_addr(reg_center_addr),
      _online_callback(std::move(online_callback)),
      _offline_callback(std::move(offline_callback))
{
}
std::string SvcProvider::make_key()
{

    std::stringstream ss;
    ss << "/" << _svc_name << "/" << _instance_id;
    return ss.str();
}
bool SvcProvider::registry()
{

    etcd::Client client(_reg_center_addr);

    wait_for_connection(client);
    auto lease_resp = client.leasegrant(3).get();
    if (!lease_resp.is_ok())
    {
        std::cerr
            << "[Etcd] 申请租约失败: "
            << lease_resp.error_code()
            << " - "
            << lease_resp.error_message()
            << std::endl;

        return false;
    }
    auto lease_id = lease_resp.value().lease();

    std::cout << "[Put] 租约 ID: "
              << lease_id << std::endl;
    auto resp = client.put(make_key(), _svc_addr, lease_id).get();
    if (!resp.is_ok())
    {
        std::cerr
            << "[Etcd] 写入数据失败: "
            << resp.error_code()
            << " - "
            << resp.error_message()
            << std::endl;

        return false;
    }
    std::cout << "[Put] 注册成功: "
              << make_key() << " = " << _svc_addr
              << std::endl;


    auto handler = [](std::exception_ptr eptr)
    {
        if (!eptr)
        {
            return;
        }

        try
        {
            std::rethrow_exception(eptr);
        }
        catch (const std::exception& e)
        {
            std::cerr << "[KeepAlive] 续租异常: "
                      << e.what()
                      << std::endl;
        }
    };

    _keepalive = std::make_shared<etcd::KeepAlive>(_reg_center_addr,handler,3,lease_id);
    std::cout << "[Put] KeepAlive 已启动"
              << std::endl;


    return true;
}
std::string SvcWatcher::parse_key(
    const std::string &key)
{
    std::vector<std::string>arry;
    biteutil::STR::split(key, "/", arry);
    if (arry.size() != 2)
    {
        return "";
    }
    if (key != "/" + arry[0] + "/" + arry[1])
    {
        return "";
    }

    return arry[0];
}
void SvcWatcher::callback(
    const etcd::Response &resp)
{

    if (!resp.is_ok())
    {
        std::cerr << "[Watcher] 监视出错: "
                  << resp.error_code()
                  << " - "
                  << resp.error_message()
                  << std::endl;
        return;
    }

    for (const auto& ev : resp.events())
    {
        std::string svc_name = parse_key(ev.kv().key());

        if (svc_name.empty())
        {
            continue;
        }
        if (ev.event_type() == etcd::Event::EventType::PUT)
        {
             std::string svc_addr = ev.kv().as_string();
            if (_online_callback && !svc_addr.empty())
            {
                _online_callback(svc_name, svc_addr);
            }
        }
        else if (ev.event_type() ==
                 etcd::Event::EventType::DELETE_)
        {
            std::string svc_addr = ev.prev_kv().as_string();
            if (_offline_callback && !svc_addr.empty())
            {
                _offline_callback(svc_name, svc_addr);
            }
            else if (svc_addr.empty())
            {
                std::cerr
                    << "[Watcher] DELETE 事件缺少原节点地址: "
                    << ev.kv().key()
                    << std::endl;
            }
        }
    }
}
bool SvcWatcher::watch()
{
    // TODO 1:
    // 创建 etcd::Client
    // 等待 Etcd 服务端连接成功
     etcd::Client client(_reg_center_addr);

    wait_for_connection(client);
    auto resp = client.ls("/").get();
    // TODO 2:
    // 使用 client.ls("/").get()
    // 获取已有的服务注册信息
    // 检查请求是否成功
    if (resp.is_ok())
    {
        std::cout << "[Get] 当前数据:"
                  << std::endl;

        for (size_t i = 0; i < resp.keys().size(); i++)
        {
            std::string svc_name = parse_key(resp.key(i));
            if (svc_name.empty())
            {
                continue;
            }
            std::string svc_addr = resp.value(i).as_string();
            if (_online_callback && !svc_addr.empty())
            {
                _online_callback(svc_name, svc_addr);
            }
        }
    }
    else
    {
        std::cerr
            << "[Get] 获取数据失败: "
            << resp.error_code()
            << " - "
            << resp.error_message()
            << std::endl;

        return false;
    }

    auto cb = std::bind(
        &SvcWatcher::callback,this,
        std::placeholders::_1
    );
    _watcher = std::make_shared<etcd::Watcher>(_reg_center_addr,"/",cb,true);

    if (!_watcher)
    {
        return false;
    }
    _watcher->Wait([](bool cancelled)
    {
    if (cancelled)
    {
        std::cout << "[Watcher] 监听已取消" << std::endl;
    }
    else
    {
        std::cout << "[Watcher] 监听异常结束" << std::endl;
    }
    });
    return true;
}
} // namespace bitesvc
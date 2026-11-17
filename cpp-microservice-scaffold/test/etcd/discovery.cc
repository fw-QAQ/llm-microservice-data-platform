#include "etcd.h"

#include <iostream>
#include <string>

// 服务上线回调
void online(
    std::string svc_name,
    std::string svc_addr)
{
    std::cout
        << "[Discovery] 新服务 "
        << svc_name
        << " 上线节点: "
        << svc_addr
        << std::endl;
}

// 服务下线回调
void offline(
    std::string svc_name,
    std::string svc_addr)
{
    std::cout
        << "[Discovery] 服务 "
        << svc_name
        << " 下线节点: "
        << svc_addr
        << std::endl;
}

int main()
{
    const std::string url =
        "http://127.0.0.1:2379";

    // 1. 创建服务发现对象
    bitesvc::SvcWatcher watcher(
        url,
        online,
        offline
    );

    // 2. 开始查询和监听
    if (!watcher.watch())
    {
        std::cerr << "服务发现启动失败"
                  << std::endl;

        return 1;
    }

    std::cout << "服务发现已启动"
              << std::endl;

    std::cout << "按回车键退出..."
              << std::endl;

    std::cin.get();

    return 0;
}
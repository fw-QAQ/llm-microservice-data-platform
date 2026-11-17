#include "etcd.h"

#include <iostream>
#include <string>

int main()
{
    const std::string url =
        "http://127.0.0.1:2379";

    const std::string svc_name = "user";

    const std::string svc_addr =
        "127.0.0.1:9000";

    // 1. 创建服务注册对象
    bitesvc::SvcProvider provider(
        url,
        svc_name,
        svc_addr
    );

    // 2. 注册服务
    if (!provider.registry())
    {
        std::cerr << "服务注册失败"
                  << std::endl;

        return 1;
    }

    std::cout << "[Registry] 服务注册成功"
              << std::endl;

    // 3. 保持服务注册对象存活
    std::cout << "按回车键退出..."
              << std::endl;

    std::cin.get();

    return 0;
}
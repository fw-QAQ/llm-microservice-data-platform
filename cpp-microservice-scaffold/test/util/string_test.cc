#include "util.h"

#include <iostream>
#include <vector>
#include <string>
#include <cassert>

int main()
{
    std::vector<std::string> result;

    // 测试 1：服务注册 Key
    biteutil::STR::split(
        "/user/instance001", "/", result
    );

    assert(result.size() == 2);
    assert(result[0] == "user");
    assert(result[1] == "instance001");

    // 测试 2：普通字符串
    biteutil::STR::split(
        "hello,world,cpp", ",", result
    );

    assert(result.size() == 3);
    assert(result[0] == "hello");
    assert(result[1] == "world");
    assert(result[2] == "cpp");

    // 测试 3：连续分隔符
    biteutil::STR::split(
        "a//b///c", "/", result
    );

    assert(result.size() == 3);
    assert(result[0] == "a");
    assert(result[1] == "b");
    assert(result[2] == "c");

    std::cout << "STR::split 测试全部通过"
              << std::endl;

    return 0;
}
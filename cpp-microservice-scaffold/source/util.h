#pragma once

#include <string>
#include <vector>
#include <random>
namespace biteutil
{

class STR
{
public:
    // 根据指定分隔符拆分字符串
    static void split(
        const std::string& str,
        const std::string& sep,
        std::vector<std::string>& result
    );
};

class Random
{
public:
    // 生成 32 字符的随机十六进制标识
    static std::string code();
};

} // namespace biteutil
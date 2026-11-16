#pragma once

#include <jsoncpp/json/json.h>
#include <string>

namespace bitejson
{
    // JSON 序列化
    bool serialize(const Json::Value &value, std::string *str);

    // JSON 反序列化
    bool unserialize(const std::string &str,
                     Json::Value *value,
                     std::string *err);
}
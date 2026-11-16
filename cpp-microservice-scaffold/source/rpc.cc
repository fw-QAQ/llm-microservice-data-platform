#include "rpc.h"
#include <brpc/channel.h>
#include <optional>

namespace biterpc
{

Channels::Channels()
    : _idx(0)
{
}

// 新增服务节点
void Channels::insert(const std::string &addr)
{
    std::lock_guard<std::mutex> lock(_mtx);


    ChannelPtr channel = std::make_shared<brpc::Channel>();

    brpc::ChannelOptions options;
    options.protocol = "baidu_std";
    options.timeout_ms = 30000;

    if(channel->Init(addr.c_str(), &options))
    {
        return;
    }

    _channels.push_back({addr, channel});
}

// 删除指定服务节点
void Channels::remove(const std::string &addr)
{
    std::lock_guard<std::mutex> lock(_mtx);
    for(int i = 0;i < _channels.size();i++)
    {
        if(_channels[i].first == addr)
        {
            _channels.erase(_channels.begin()+i);
            break;
        }

    }


}

// 轮询返回一个 Channel
ChannelPtr Channels::select()
{
    std::lock_guard<std::mutex> lock(_mtx);


    if(_channels.size() == 0)
    {
        return nullptr;
    }
    if(_idx >= _channels.size())
    {
        _idx %= _channels.size();
    }
    ChannelPtr channel = _channels[_idx++].second;
    return channel;
}

// 轮询返回一个服务地址
std::optional<std::string> Channels::selectAddr()
{
    std::lock_guard<std::mutex> lock(_mtx);

    if(_channels.size() == 0)
    {
        return std::nullopt;
    }
    if(_idx >= _channels.size())
    {
        _idx %= _channels.size();
    }
    std::string ret = _channels[_idx++].first;



    return ret;
}

} // namespace biterpc

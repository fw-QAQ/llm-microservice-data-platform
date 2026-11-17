#include "rpc.h"
#include <brpc/callback.h>
#include <brpc/channel.h>
#include <memory>
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
// 根据服务名称获取管理对象
Channels::ptr SvcChannels::_Channels(
    const std::string &svc_name)
{
    std::lock_guard<std::mutex> lock(_mtx);

    auto channel = _maps.find(svc_name);
    if(channel != _maps.end())
    {
        return channel->second;
    }
    return Channels::ptr();
}


// 注册需要管理的服务
void SvcChannels::setWatch(
    const std::string &svc_name)
{
    std::lock_guard<std::mutex> lock(_mtx);
    Channels::ptr channels = std::make_shared<Channels>();
    _maps.insert({svc_name,channels});
}


// 向指定服务中添加节点
void SvcChannels::addNode(
    const std::string &svc_name,
    const std::string &addr)
{
    Channels::ptr channels = _Channels(svc_name);
    if(channels == nullptr)
    {
        return;
    }
    channels->insert(addr);
}


// 删除指定服务中的节点
void SvcChannels::delNode(
    const std::string &svc_name,
    const std::string &addr)
{

    Channels::ptr channels = _Channels(svc_name);
    if(channels == nullptr)
    {
        return;
    }
    channels->remove(addr);
}


// 获取指定服务的 Channel
ChannelPtr SvcChannels::getNode(
    const std::string &svc_name)
{

    Channels::ptr channels = _Channels(svc_name);
    if(channels != nullptr)
    {
        return channels->select();
    }
    return ChannelPtr();
}

// 获取指定服务的节点地址
std::optional<std::string> SvcChannels::getNodeAddr(
    const std::string &svc_name)
{

    Channels::ptr channels = _Channels(svc_name);
    if(channels != nullptr)
    {
        return channels->selectAddr();
    }
    return std::nullopt;
}

// 创建异步回调对象
google::protobuf::Closure* ClosureFactory::create(
    callback_t&& cb)
{

    Object::ptr obj = std::make_shared<Object>();
    obj->callback = std::move(cb);
    return brpc::NewCallback(ClosureFactory::asyncCallback,obj);
}


// 执行用户提供的业务回调
void ClosureFactory::asyncCallback(const Object::ptr obj)
{

    obj->callback();
}

std::shared_ptr<brpc::Server> RpcServerFactory::create(
    int port,
    google::protobuf::Service* svc)
{

    brpc::ServerOptions opt;
    opt.idle_timeout_sec = -1;
    std::shared_ptr<brpc::Server> sev =std::make_shared<brpc::Server>();

    if (sev->AddService(
            svc,
            brpc::SERVER_OWNS_SERVICE) != 0)
    {
        std::cerr << "add service failed!\n";
        return  nullptr;
    }
    if (sev->Start(port,&opt) !=0)
    {
        std::cerr << "Fail to start EchoServer\n";
        return nullptr;
    }
    return sev;
}

} // namespace biterpc
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <optional>

extern "C"
{
#include <fastdfs/fdfs_client.h>
#include <fastcommon/logger.h>
}
namespace bitefdfs
{

struct fdfs_settings
{
    std::vector<std::string> trackers;

    int connect_timeout = 30;

    int network_timeout = 30;

    bool use_connection_pool = true;

    int connection_pool_max_idle_time = 3600;
};

class FDFSClient
{
public:
    // 初始化客户端
    static void init(const fdfs_settings &settings);

    // 释放全局资源
    static void destroy();

    // 从内存缓冲区上传
    static std::optional<std::string> upload_from_buff(
        const std::string &buff);

    // 从本地文件上传
    static std::optional<std::string> upload_from_file(
        const std::string &path);

    // 下载到内存缓冲区
    static bool download_to_buff(
        const std::string &file_id,
        std::string &buff);

    // 下载到本地文件
    static bool download_to_file(
        const std::string &file_id,
        const std::string &path);

    // 删除文件
    static bool remove(const std::string &file_id);
};

} // namespace bitefdfs
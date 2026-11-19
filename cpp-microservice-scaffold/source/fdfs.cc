#include <cstdint>
#include <iostream>
#include <optional>
#include <sstream>
#include <cstdlib>
#include <memory>

#include "fdfs.h"

#include <fastcommon/logger.h>
#include <fastdfs/fdfs_client.h>
namespace bitefdfs
{

void FDFSClient::init(const fdfs_settings &settings)
{
    g_log_context.log_level = LOG_ERR;
    log_init();

    // 检查是否配置 Tracker
    if (settings.trackers.empty())
    {
        std::cerr << "没有配置 Tracker 服务器" << std::endl;
        std::abort();
    }

    std::stringstream ss;

    for (const auto& tracker : settings.trackers)
    {
        ss << "tracker_server = " << tracker << "\n";
    }

    ss << "connect_timeout = "
       << settings.connect_timeout << "\n";

    ss << "network_timeout = "
       << settings.network_timeout << "\n";

    ss << "use_connection_pool = "
       << settings.use_connection_pool << "\n";

    ss << "connection_pool_max_idle_time = "
       << settings.connection_pool_max_idle_time << "\n";

    std::string config = ss.str();

    int ret = fdfs_client_init_from_buffer(config.c_str());

    if (ret != 0)
    {
        std::cerr
            << "初始化全局配置失败: "
            << STRERROR(ret)
            << std::endl;

        std::abort();
    }
}

void FDFSClient::destroy()
{
    fdfs_client_destroy();
}

std::optional<std::string> FDFSClient::upload_from_buff(
    const std::string& buff)
{

    ConnectionInfo* tracker_server =
        tracker_get_connection();
    if (tracker_server == nullptr)
    {
        std::cerr << "[FastDFS] 获取 Tracker 服务器连接失败"
              << std::endl;

        return std::nullopt;
    }

    char file_id[256] = {0};
    int ret = storage_upload_by_filebuff1(
    tracker_server,  // 1. Tracker 连接
    nullptr,         // 2. Storage 连接
    0,               // 3. 存储路径索引
    buff.data(),     // 4. 文件数据首地址
    buff.size(),     // 5. 文件数据字节数
    nullptr,         // 6. 文件扩展名
    nullptr,         // 7. 元数据数组
    0,               // 8. 元数据数量
    nullptr,         // 9. Group 名称
    file_id          // 10. 输出 File ID
);
    tracker_close_connection(tracker_server);
    if (ret != 0)
    {
        std::cerr
            << "[FastDFS] 文件上传失败: "
            << STRERROR(ret)
            << std::endl;
        return std::nullopt;
    }
    return std::string(file_id);
}

std::optional<std::string> FDFSClient::upload_from_file(
    const std::string& path)
{

      ConnectionInfo *tracker_server = tracker_get_connection();

  if (tracker_server == nullptr) {
    std::cerr << "[Test] Tracker 连接失败" << std::endl;
     return std::nullopt;
  }

  char file_id[256] = {0};

  int ret = storage_upload_by_filename1(
    tracker_server,
    nullptr,
    0,
    path.c_str(),
    nullptr,
    nullptr,
    0,
    nullptr,
    file_id
);
    tracker_close_connection(tracker_server);
    if (ret != 0)
    {
        std::cerr
            << "[FastDFS] 文件上传失败: "
            << STRERROR(ret)
            << std::endl;
        return std::nullopt;
    }
    return std::string(file_id);
}
bool FDFSClient::download_to_buff(
    const std::string& file_id,
    std::string& buff)
{
    ConnectionInfo* tracker_server = tracker_get_connection();

    if (tracker_server == nullptr)
    {
        std::cerr << "[FastDFS] 获取 Tracker 连接失败"
                  << std::endl;
        return false;
    }

    char* file_buff = nullptr;
    int64_t file_size = 0;

    int ret = storage_download_file_to_buff1(
        tracker_server,
        nullptr,
        file_id.c_str(),
        &file_buff,
        &file_size
    );

    tracker_close_connection(tracker_server);

    // 使用 RAII 管理 SDK 分配的内存
    std::unique_ptr<char, decltype(&std::free)> guard(
        file_buff,
        &std::free
    );

    if (ret != 0)
    {
        std::cerr << "[FastDFS] 文件下载失败: "
                  << STRERROR(ret)
                  << std::endl;
        return false;
    }

    if (file_size < 0 ||
        (file_size > 0 && file_buff == nullptr))
    {
        std::cerr << "[FastDFS] 下载数据异常"
                  << std::endl;
        return false;
    }

    std::string result;

    if (file_size > 0)
    {
        result.assign(
            file_buff,
            static_cast<size_t>(file_size)
        );
    }

    buff.swap(result);

    return true;
}
bool FDFSClient::download_to_file(
    const std::string& file_id,
    const std::string& path)
{

    ConnectionInfo* tracker_server =
        tracker_get_connection();

    if (tracker_server == nullptr)
    {
        std::cerr << "获取 Tracker 连接失败"
                  << std::endl;
        return false;
    }

    int64_t file_size = 0;

    int ret = storage_download_file_to_file1(tracker_server,
        nullptr,
        file_id.c_str(),
        path.c_str(),
        &file_size
    );
    tracker_close_connection(tracker_server);

    if (ret != 0)
    {
        std::cerr
            << "[FastDFS] 文件下载失败: "
            << STRERROR(ret)
            << std::endl;
        return false;
    }
    std::cout << "[FastDFS] 文件下载成功，文件大小: " << file_size << " 字节" << std::endl;

    return true;
}
bool FDFSClient::remove(const std::string& file_id)
{

    ConnectionInfo* tracker_server =
        tracker_get_connection();

    if (tracker_server == nullptr)
    {
        std::cerr << "[FastDFS] 获取 Tracker 连接失败"
                  << std::endl;
        return false;
    }

    int ret = storage_delete_file1(tracker_server,
        nullptr,
        file_id.c_str()
    );

    tracker_close_connection(tracker_server);

    if (ret != 0)
    {
        std::cerr
            << "[FastDFS] 文件删除失败: "
            << STRERROR(ret)
            << std::endl;
        return false;
    }
    return true;
}
} // namespace bitefdfs
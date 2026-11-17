
#include <iostream>
#include <optional>
#include <sstream>
#include <cstdlib>

#include "fdfs.h"
#include <fastcommon/logger.h>
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
    // TODO 1：获取 Tracker 连接
    // 如果连接失败，返回 std::nullopt
      ConnectionInfo *tracker_server = tracker_get_connection();

  if (tracker_server == nullptr) {
    std::cerr << "[Test] Tracker 连接失败" << std::endl;
     return std::nullopt;
  }
    // TODO 2：创建 file_id 缓冲区
  char file_id[256] = {0};
    // TODO 3：调用 storage_upload_by_filename1()
    // 使用 path.c_str() 作为本地文件路径
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

} // namespace bitefdfs
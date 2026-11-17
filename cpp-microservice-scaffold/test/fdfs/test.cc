#include <iostream>
#include <string>
#include <vector>

// 引入 FastDFS 客户端封装
#include "fdfs.h"

int main() {
  // 1. 配置 Tracker
  bitefdfs::fdfs_settings settings;

  settings.trackers = {"127.0.0.1:22122"};

  // 2. 初始化 FastDFS
  bitefdfs::FDFSClient::init(settings);

  std::cout << "[Test] FDFSClient 初始化完成" << std::endl;

  // 3. 验证 Tracker 连接
  ConnectionInfo *tracker = tracker_get_connection();

  if (tracker == nullptr) {
    std::cerr << "[Test] Tracker 连接失败" << std::endl;

    bitefdfs::FDFSClient::destroy();

    return 1;
  }

  std::cout << "[Test] Tracker 连接成功" << std::endl;

  // 4. 关闭 Tracker 连接
  tracker_close_connection(tracker);
  std::string data = "Hello FastDFS Buffer!";

  auto file_id = bitefdfs::FDFSClient::upload_from_buff(data);

  if (!file_id.has_value()) {
    std::cerr << "[Test] 内存上传失败" << std::endl;
    bitefdfs::FDFSClient::destroy();
    return 1;
  }

  std::cout << "[Test] 内存上传成功: " << file_id.value() << std::endl;

  file_id = bitefdfs::FDFSClient::upload_from_file(
    "./upload_test.txt");

if (!file_id.has_value())
{
    std::cerr << "[Test] 文件上传失败" << std::endl;
    bitefdfs::FDFSClient::destroy();
    return 1;
}

std::cout << "[Test] 文件上传成功: "
          << file_id.value()
          << std::endl;
  // 5. 释放全局资源
  bitefdfs::FDFSClient::destroy();

  std::cout << "[Test] FDFSClient 资源释放完成" << std::endl;

  return 0;
}

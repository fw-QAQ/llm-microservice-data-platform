#include <iostream>
#include <string>
#include <vector>

// 按课件建议，最后引入 fdfs.h
#include "fdfs.h"

int main() {
  // 1. 配置 Tracker
  bitefdfs::fdfs_settings settings;

  settings.trackers = {"dev-tracker:22122"};

  // 2. 初始化 FastDFS
  bitefdfs::FDFSClient::init(settings);

  std::cout << "[Test] FDFSClient 初始化完成" << std::endl;

  // 3. 验证 Tracker 连接

std::string data = "Hello FastDFS Buffer!";

auto buff_file_id =
    bitefdfs::FDFSClient::upload_from_buff(data);

if (!buff_file_id.has_value())
{
    std::cerr << "[FastDFS] 内存上传失败" << std::endl;
    bitefdfs::FDFSClient::destroy();
    return 1;
}

std::cout << "[Test] 内存上传成功: "
          << buff_file_id.value()
          << std::endl;
auto local_file_id =
    bitefdfs::FDFSClient::upload_from_file("./upload_test.txt");

if (!local_file_id.has_value())
{
    std::cerr << "[FastDFS] 文件上传失败" << std::endl;
    bitefdfs::FDFSClient::destroy();
    return 1;
}
//统一文件上传成功日志格式，添加[FastDFS]前缀
std::cout << "[FastDFS] 文件上传成功: "
          << local_file_id.value()
          << std::endl;

std::string downloaded;

bool ok = bitefdfs::FDFSClient::download_to_buff(
    buff_file_id.value(),
    downloaded
);

if (!ok)
{
    std::cerr << "[FastDFS] 内存下载失败" << std::endl;
    bitefdfs::FDFSClient::destroy();
    return 1;
}

if (downloaded != data)
{
    std::cerr << "[FastDFS] 下载内容不一致" << std::endl;
    bitefdfs::FDFSClient::destroy();
    return 1;
}

std::cout << "[FastDFS] 内存下载成功" << std::endl;
std::cout << "[FastDFS] 数据一致性校验通过" << std::endl;

bool file_download_ok =
    bitefdfs::FDFSClient::download_to_file(
        local_file_id.value(),
        "./downloaded_upload_test.txt"
    );

if (!file_download_ok)
{
    std::cerr << "[FastDFS] 文件下载失败" << std::endl;

    bitefdfs::FDFSClient::destroy();
    return 1;
}

std::cout << "[FastDFS] 文件下载成功" << std::endl;

// 1. 删除之前上传的本地文件
bool remove_ok = bitefdfs::FDFSClient::remove(
    local_file_id.value()
);

if (!remove_ok)
{
    std::cerr << "[FastDFS] 文件删除失败" << std::endl;
    bitefdfs::FDFSClient::destroy();
    return 1;
}
std::cout << "[FastDFS] 文件删除成功" << std::endl;


// 2. 尝试重新下载已经删除的文件
std::string deleted_data;

bool download_ok = bitefdfs::FDFSClient::download_to_buff(
    local_file_id.value(),
    deleted_data
);

if (download_ok)
{
    std::cerr << "[FastDFS] 错误：删除后仍能下载文件"
              << std::endl;

    bitefdfs::FDFSClient::destroy();
    return 1;
}
std::cout << "[FastDFS] 删除验证通过：文件已不存在"
          << std::endl;
  // 5. 释放全局资源
  bitefdfs::FDFSClient::destroy();

  std::cout << "[FastDFS] FDFSClient 资源释放完成" << std::endl;

  return 0;
}
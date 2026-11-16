#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

#include <memory>
#include <string>

namespace bitelog
{
    struct log_settings
    {
        bool async = false;
        int level = 1;
        std::string format = "[%H:%M:%S][%-7l]: %v";
        std::string path = "stdout";
    };

    // 全局日志器声明
    extern std::shared_ptr<spdlog::logger> g_logger;

    // 日志初始化接口
    extern void bitelog_init(
        const log_settings &settings = log_settings());

    // 文件名、行号格式
    #define FMT_PREFIX "[{}:{}]: "


    #define DBG(fmt, ...) bitelog::g_logger->debug(FMT_PREFIX  fmt, __FILE__, __LINE__,##__VA_ARGS__);

    #define INF(fmt,...) bitelog::g_logger->info(FMT_PREFIX fmt, __FILE__, __LINE__,##__VA_ARGS__);

    #define WRN(fmt,...) bitelog::g_logger->warn(FMT_PREFIX  fmt, __FILE__, __LINE__,##__VA_ARGS__);

    #define ERR(fmt,...) bitelog::g_logger->error(FMT_PREFIX  fmt, __FILE__, __LINE__,##__VA_ARGS__);
}
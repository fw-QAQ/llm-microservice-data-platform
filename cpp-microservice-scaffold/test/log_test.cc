#include <log.h>
#include <gflags/gflags.h>

DEFINE_bool(log_async, true, "是否启用异步日志");
DEFINE_int32(log_level, 1, "日志输出等级");
DEFINE_string(log_format, "[%H:%M:%S][%-7l]%v", "日志输出格式");
DEFINE_string(log_path, "stdout", "日志输出路径");

int main(int argc, char *argv[])
{
    // 解析命令行参数
    google::ParseCommandLineFlags(&argc, &argv, true);

    // 初始化日志配置
    bitelog::log_settings settings;

    settings.async = FLAGS_log_async;
    settings.level = FLAGS_log_level;
    settings.format = FLAGS_log_format;
    settings.path = FLAGS_log_path;

    // 初始化日志器
    bitelog::bitelog_init(settings);

    // 测试日志输出宏
    DBG("{}今年{}岁", "小明", 18);
    INF("{}今年{}岁", "小红", 19);
    WRN("{}今年{}岁", "小刚", 20);
    ERR("{}今年{}岁", "小李", 21);

    DBG("HELLO");

    return 0;
}
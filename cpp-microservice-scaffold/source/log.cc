#include "log.h"

namespace bitelog
{
    std::shared_ptr<spdlog::logger> g_logger;

    void bitelog_init(const log_settings &settings)
    {
        if (settings.async == true)
        {
            if (settings.path == "stdout")
            {
                g_logger = spdlog::stdout_color_mt<spdlog::async_factory>("async_stdout_logger");
            }
            else
            {

                g_logger = spdlog::basic_logger_mt<spdlog::async_factory>("async_file_logger",settings.path.c_str());
            }
        }
        else
        {
            if (settings.path == "stdout")
            {
                g_logger = spdlog::stdout_color_mt("stdout_logger");
            }
            else
            {
                g_logger = spdlog::basic_logger_mt("file_logger", settings.path);
            }
        }

        g_logger->set_level(spdlog::level::level_enum(settings.level));
        g_logger->set_pattern(settings.format);
    }
}
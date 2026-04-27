/**
 * @file logging.hpp
 * @brief Project-wide logging facade backed by spdlog.
 *
 * Use the ATE_LOG_* macros so the call site (file/line/function) is captured
 * automatically. Log level honours the `SPDLOG_LEVEL` environment variable.
 */
#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/cfg/env.h>
#include <memory>

namespace ate::log {

/// Returns the shared "ate" logger, creating it on first use.
inline std::shared_ptr<spdlog::logger>& logger() {
    static std::shared_ptr<spdlog::logger> instance = [] {
        auto lg = spdlog::stderr_color_mt("ate");
        lg->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
        lg->set_level(spdlog::level::info);
        spdlog::cfg::load_env_levels();   // honour SPDLOG_LEVEL=debug, etc.
        return lg;
    }();
    return instance;
}

inline void set_level(spdlog::level::level_enum lvl) { logger()->set_level(lvl); }

} // namespace ate::log

#define ATE_LOG_TRACE(...)    SPDLOG_LOGGER_TRACE(::ate::log::logger(),    __VA_ARGS__)
#define ATE_LOG_DEBUG(...)    SPDLOG_LOGGER_DEBUG(::ate::log::logger(),    __VA_ARGS__)
#define ATE_LOG_INFO(...)     SPDLOG_LOGGER_INFO(::ate::log::logger(),     __VA_ARGS__)
#define ATE_LOG_WARN(...)     SPDLOG_LOGGER_WARN(::ate::log::logger(),     __VA_ARGS__)
#define ATE_LOG_ERROR(...)    SPDLOG_LOGGER_ERROR(::ate::log::logger(),    __VA_ARGS__)
#define ATE_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(::ate::log::logger(), __VA_ARGS__)

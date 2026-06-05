#pragma once

// ============================================================
//  轻量日志宏 —— 零依赖，替代 spdlog
//  用法和 spdlog 一样：LOG_INFO("xxx {}", val);
//  通过 LOG_LEVEL 控制输出级别
// ============================================================

#include <iostream>
#include <string>

namespace logdetail {

// 日志级别（数值越大越详细）
enum class Level : int {
    OFF   = 0,
    ERROR = 1,
    WARN  = 2,
    INFO  = 3,
    DEBUG = 4,
    TRACE = 5,
};

// 当前日志级别（编译期或运行期可改）
inline Level g_level = Level::INFO;

inline const char* levelName(Level lv) {
    switch (lv) {
        case Level::ERROR: return "\033[31mERROR\033[0m";  // 红色
        case Level::WARN:  return "\033[33mWARN\033[0m ";  // 黄色
        case Level::INFO:  return "\033[32mINFO\033[0m ";  // 绿色
        case Level::DEBUG: return "\033[36mDEBUG\033[0m";  // 青色
        case Level::TRACE: return "TRACE";
        default:           return "";
    }
}

// 简陋的 fmt 替代：把 {} 依次替换为参数
inline void formatHelper(std::string&, size_t) {}

template <typename T, typename... Args>
void formatHelper(std::string& s, size_t pos, const T& val, const Args&... rest) {
    auto brace = s.find("{}", pos);
    if (brace == std::string::npos) return;
    std::string rep;
    if constexpr (std::is_same_v<T, std::string>) {
        rep = val;
    } else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>) {
        rep = val;
    } else if constexpr (std::is_arithmetic_v<T>) {
        rep = std::to_string(val);
    } else {
        rep = "?";
    }
    s.replace(brace, 2, rep);
    formatHelper(s, brace + rep.size(), rest...);
}

template <typename... Args>
std::string format(const std::string& fmt, const Args&... args) {
    std::string s = fmt;
    formatHelper(s, 0, args...);
    return s;
}

} // namespace logdetail

// ---- 日志宏 ----
#define LOG_ERROR(fmt, ...) do { \
    if (::logdetail::g_level >= ::logdetail::Level::ERROR) \
        std::cerr << "[" << ::logdetail::levelName(::logdetail::Level::ERROR) << "] " \
                  << ::logdetail::format(fmt, ##__VA_ARGS__) << std::endl; \
} while(0)

#define LOG_WARN(fmt, ...) do { \
    if (::logdetail::g_level >= ::logdetail::Level::WARN) \
        std::cerr << "[" << ::logdetail::levelName(::logdetail::Level::WARN) << "] " \
                  << ::logdetail::format(fmt, ##__VA_ARGS__) << std::endl; \
} while(0)

#define LOG_INFO(fmt, ...) do { \
    if (::logdetail::g_level >= ::logdetail::Level::INFO) \
        std::cout << "[" << ::logdetail::levelName(::logdetail::Level::INFO) << "] " \
                  << ::logdetail::format(fmt, ##__VA_ARGS__) << std::endl; \
} while(0)

#define LOG_DEBUG(fmt, ...) do { \
    if (::logdetail::g_level >= ::logdetail::Level::DEBUG) \
        std::cout << "[" << ::logdetail::levelName(::logdetail::Level::DEBUG) << "] " \
                  << ::logdetail::format(fmt, ##__VA_ARGS__) << std::endl; \
} while(0)

#define LOG_TRACE(fmt, ...) do { \
    if (::logdetail::g_level >= ::logdetail::Level::TRACE) \
        std::cout << "[" << ::logdetail::levelName(::logdetail::Level::TRACE) << "] " \
                  << ::logdetail::format(fmt, ##__VA_ARGS__) << std::endl; \
} while(0)

// 设置日志级别
#define SET_LOG_LEVEL(lv) (::logdetail::g_level = (lv))

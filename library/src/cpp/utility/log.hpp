#pragma once

#include <android/log.h>
#include <fmt/format.h>

// Android logging utility
// Using fmt for template parsing, so you can use either:
// - printf style: debug("string: %s, number: %d", "ha", 3) => "D/libGDX-Oboe string: ha, number: 3"
// - python style: info("string: {}, number: {}", "ha", 3) => "I/libGDX-Oboe string: ha, number: 3"

// <><><><><><><><><><><><><><><><><><><><>
//                Details
// <><><><><><><><><><><><><><><><><><><><>

enum class log_level { debug, info, error, warn };

template<class ... Params>
void log(log_level level, std::string_view format, Params... params) {
    auto message = fmt::format(format, params...);
    auto mapped_level = ANDROID_LOG_DEBUG;

    switch(level) {
        case log_level::debug: mapped_level = ANDROID_LOG_DEBUG; break;
        case log_level::info: mapped_level = ANDROID_LOG_INFO; break;
        case log_level::error: mapped_level = ANDROID_LOG_ERROR; break;
        case log_level::warn: mapped_level = ANDROID_LOG_WARN; break;
    }

    __android_log_print(mapped_level, "libGDX-Oboe", "%s", message.c_str());
}

inline void android_assert(bool cond, std::string_view msg) {
    if(!cond) {
        __android_log_assert(msg.data(), "libGDX-Oboe", nullptr);
    }
}

// <><><><><><><><><><><><><><><><><><><><>
//                User API
// <><><><><><><><><><><><><><><><><><><><>

template<class ... Params>
inline void debug(std::string_view format, Params... params) {
    log(log_level::debug, format, params...);
}

template<class ... Params>
inline void info(std::string_view format, Params... params) {
    log(log_level::info, format, params...);
}

template<class ... Params>
inline void error(std::string_view format, Params... params) {
    log(log_level::error, format, params...);
}

template<class ... Params>
inline void warn(std::string_view format, Params... params) {
    log(log_level::warn, format, params...);
}

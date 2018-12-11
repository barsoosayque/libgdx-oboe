#pragma once

#include <android/log.h>
#include <fmt/format.h>

// Android logging utility
// Using fmt for template parsing, so you can use either:
// - printf style: debug("string: %s, number: %d", "ha", 3) => "[DEBUG] string: ha, number: 3"
// - python style: info("string: {}, number: {}", "ha", 3) => "[INFO] string: ha, number: 3"

// <><><><><><><><><><><><><><><><><><><><>
//                Details
// <><><><><><><><><><><><><><><><><><><><>

enum class log_level { debug, info, error };

template<class ... Params>
void log(log_level p_level, std::string_view p_template, Params... p_params) {
    auto message = fmt::format(p_template, p_params...);
    auto level = ANDROID_LOG_DEBUG;

    switch(p_level) {
        case log_level::debug: level = ANDROID_LOG_DEBUG; break;
        case log_level::info: level = ANDROID_LOG_INFO; break;
        case log_level::error: level = ANDROID_LOG_ERROR; break;
    }

    __android_log_print(level, "[Libgdx Oboe]", "%s", message.c_str());
}

// <><><><><><><><><><><><><><><><><><><><>
//                User API
// <><><><><><><><><><><><><><><><><><><><>

template<class ... Params>
inline void debug(std::string_view p_template, Params... p_params) {
    log(log_level::debug, p_template, p_params...);
}

template<class ... Params>
inline void info(std::string_view p_template, Params... p_params) {
    log(log_level::info, p_template, p_params...);
}

template<class ... Params>
inline void error(std::string_view p_template, Params... p_params) {
    log(log_level::error, p_template, p_params...);
}

#pragma once
#include "log.hpp"
#include <oboe/Oboe.h>

/// @note: message should contain a %s
inline void check(oboe::Result p_result, std::string_view p_msg) {
    if (p_result != oboe::Result::OK) {
        error(p_msg, oboe::convertToText(p_result));
    }
}
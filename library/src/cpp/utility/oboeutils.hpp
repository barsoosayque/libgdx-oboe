#pragma once

#include "log.hpp"
#include <oboe/Oboe.h>

/// @note: message should contain {}
inline bool check(oboe::Result result, std::string_view msg) {
    if (result != oboe::Result::OK) {
        error(msg, oboe::convertToText(result));
        return false;
    }
    return true;
}
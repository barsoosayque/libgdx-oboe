#pragma once

#include <vector>
#include <samplerate.h>
#include <cstdint>

inline std::vector<float> to_float(const std::vector<int16_t> &in) {
    std::vector<float> out(in.size());
    src_short_to_float_array(in.data(), out.data(), out.size());
    return out;
}

inline std::vector<int16_t> to_int16(const std::vector<float> &in) {
    std::vector<int16_t> out(in.size());
    src_float_to_short_array(in.data(), out.data(), out.size());
    return out;
}

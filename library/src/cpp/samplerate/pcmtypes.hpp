#pragma once
#include <vector>
#include <samplerate.h>
#include <cstdint>

inline std::vector<float> to_float(const std::vector<int16_t>& p_in) {
    std::vector<float> out(p_in.size());
    src_short_to_float_array(p_in.data(), out.data(), out.capacity());
    return out;
}

inline std::vector<int16_t> to_int16(const std::vector<float>& p_in) {
    std::vector<int16_t> out(p_in.size());
    src_float_to_short_array(p_in.data(), out.data(), out.capacity());
    return out;
}

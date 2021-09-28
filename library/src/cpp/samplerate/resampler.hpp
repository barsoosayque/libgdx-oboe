#pragma once

#include <vector>
#include <samplerate.h>
#include <cstdint>
#include <memory>

namespace {
struct resampler_state_deleter {
    void operator()(SRC_STATE *ptr) {
        src_delete(ptr);
    }
};

using src_state_ptr = std::unique_ptr<SRC_STATE, resampler_state_deleter>;
}

class resampler {
public:
    /// @see http://www.mega-nerd.com/SRC/api_misc.html#Converters
    enum class converter : int {
        sinc_best = SRC_SINC_BEST_QUALITY,
        sinc_medium = SRC_SINC_MEDIUM_QUALITY,
        sinc_low = SRC_SINC_FASTEST,
        zero_order_hold = SRC_ZERO_ORDER_HOLD,
        linear = SRC_LINEAR
    };

    resampler(resampler::converter converter, int8_t channels, float ratio);
    resampler(const resampler &) = delete;
    resampler &operator=(const resampler &) = delete;
    resampler(resampler &&) noexcept;
    resampler &operator=(resampler &&) noexcept;

    void ratio(float ratio);
    float ratio() const;

    void reset();

    int process(std::vector<float>::const_iterator begin,
                std::vector<float>::const_iterator end,
                std::vector<float>::iterator output,
                int requested_frames);

private:
    SRC_DATA m_data;
    src_state_ptr m_state;
    int8_t m_channels;

    // internal variables
    int m_len;
};
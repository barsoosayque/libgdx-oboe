#pragma once
#include <vector>
#include <samplerate.h>
#include <cstdint>

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

        resampler(resampler::converter p_converter, int8_t p_channels, float p_ratio);
        resampler(const resampler&);
        ~resampler();

        void ratio(float p_ratio);
        float ratio();

        std::vector<int16_t> process(std::vector<int16_t>::iterator p_begin,
                                     std::vector<int16_t>::iterator p_end,
                                     bool p_last = false);

    private:
        void check_error();

        SRC_STATE* m_state;
        SRC_DATA* m_data;
        int m_error;
        converter m_converter;
        int8_t m_channels;

        std::vector<float> float_buf, float_out;
};
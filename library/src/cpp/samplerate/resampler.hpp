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

        resampler(resampler::converter converter, int8_t channels, float ratio);
        // no copy (todo: copy constructor)
        resampler(const resampler&) = delete;
        resampler& operator=(const resampler&) = delete;
        resampler(resampler&&);
        resampler& operator=(resampler&&);
        ~resampler();

        void ratio(float ratio);
        float ratio() const;

        void reset();

        int process(std::vector<float>::const_iterator begin,
                    std::vector<float>::const_iterator end,
                    std::vector<float>::iterator output,
                    int requested_frames);

    private:
        SRC_STATE* m_state;
        SRC_DATA m_data;
        int8_t m_channels;

        // internal variables
        int len;
};
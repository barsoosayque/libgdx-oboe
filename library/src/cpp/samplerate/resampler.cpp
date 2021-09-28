#include "resampler.hpp"
#include "../utility/log.hpp"
#include "../utility/exception.hpp"

resampler::resampler(resampler::converter converter, int8_t channels, float ratio)
        : m_data(SRC_DATA{ .src_ratio = ratio })
        , m_channels(channels)
        , m_len(0) {
    int err = 0;
    m_state = src_state_ptr{ src_new(static_cast<int>(converter), channels, &err) };
    if (err) {
        throw_exception("resampler::resampler error: {}", src_strerror(err));
        m_state = nullptr;
    }
}

resampler::resampler(resampler &&other) noexcept {
    *this = std::move(other);
}

resampler &resampler::operator=(resampler &&other) noexcept {
    m_data = std::exchange(other.m_data, SRC_DATA{});
    m_state = std::exchange(other.m_state, nullptr);
    m_channels = std::exchange(other.m_channels, 0);
    m_len = std::exchange(other.m_len, 0);
    return *this;
}

float resampler::ratio() const {
    return static_cast<float>(m_data.src_ratio);
}

void resampler::ratio(float ratio) {
    m_data.src_ratio = ratio;
}

void resampler::reset() {
    src_reset(m_state.get());
}

int resampler::process(std::vector<float>::const_iterator begin,
                       std::vector<float>::const_iterator end,
                       std::vector<float>::iterator output, int requested_frames) {
    if (m_state == nullptr) {
        m_len = std::distance(begin, end);
        m_len = m_len < requested_frames * m_channels ? m_len : requested_frames * m_channels;
        std::copy(begin, std::next(begin, m_len), output);
        return m_len;
    } else {
        m_len = std::distance(begin, end) / m_channels;

        m_data.data_in = &(*begin);
        m_data.data_out = &(*output);
        m_data.input_frames = m_len;
        m_data.output_frames = requested_frames;
        m_data.end_of_input = requested_frames <= m_len;

        if (int err = src_process(m_state.get(), &m_data)) {
            throw_exception("resampler::process error: {}", src_strerror(err));
        }

        return m_data.input_frames_used;
    }
}

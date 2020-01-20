#include "resampler.hpp"
#include "../utility/log.hpp"

resampler::resampler(resampler::converter converter, int8_t channels, float ratio)
    : m_channels(channels) {
    int err = 0;
    m_state = src_new(static_cast<int>(converter), channels, &err);
    if(err) {
        error("resampler::resampler error: {}", src_strerror(err));
        m_state = nullptr;
    }
    m_data = SRC_DATA { .src_ratio = ratio };
}

resampler::resampler(resampler&& other)
    : m_channels(other.m_channels)
{
    m_state = other.m_state;
    other.m_state = nullptr;
    m_data = other.m_data;
}

resampler& resampler::operator=(resampler&& other) {
    m_state = other.m_state;
    other.m_state = nullptr;
    m_data = other.m_data;
    m_channels = other.m_channels;
    return *this;
}

resampler::~resampler() {
    if(m_state != nullptr) {
        src_delete(m_state);
    }
}

float resampler::ratio() const {
    return static_cast<float>(m_data.src_ratio);
}

void resampler::ratio(float ratio) {
    m_data.src_ratio = ratio;
}

void resampler::reset() {
    src_reset(m_state);
}

int resampler::process(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end,
                      std::vector<float>::iterator output, int requested_frames) {
    if(m_state == nullptr) {
        len = std::distance(begin, end);
        len = len < requested_frames*m_channels ? len : requested_frames*m_channels;
        std::copy(begin, std::next(begin, len), output);
        return len;
    } else {
        len = std::distance(begin, end) / m_channels;

        m_data.data_in = &(*begin);
        m_data.data_out = &(*output);
        m_data.input_frames = len;
        m_data.output_frames = requested_frames;
        m_data.end_of_input = requested_frames <= len;

        if (int err = src_process(m_state, &m_data)) {
            error("resampler::process error: {}", src_strerror(err));
        }

        return m_data.input_frames_used;
    }
}

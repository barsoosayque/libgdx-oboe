#include "resampler.hpp"
#include "../utility/log.hpp"

resampler::resampler(resampler::converter p_converter, int8_t p_channels, float p_ratio)
    : m_channels(p_channels) {
    int err = 0;
    m_state = src_new(static_cast<int>(p_converter), p_channels, &err);
    if(err) {
        error("resampler::resampler error: {}", src_strerror(err));
        m_state = nullptr;
    }
    m_data = SRC_DATA { .src_ratio = p_ratio };
}

resampler::resampler(resampler&& p_other)
    : m_channels(p_other.m_channels)
{
    m_state = p_other.m_state;
    p_other.m_state = nullptr;
    m_data = p_other.m_data;
}

resampler& resampler::operator=(resampler&& p_other) {
    m_state = p_other.m_state;
    p_other.m_state = nullptr;
    m_data = p_other.m_data;
    m_channels = p_other.m_channels;
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

void resampler::ratio(float p_ratio) {
    m_data.src_ratio = p_ratio;
}

void resampler::reset() {
    src_reset(m_state);
}

int resampler::process(std::vector<float>::const_iterator p_begin, std::vector<float>::const_iterator p_end,
                      std::vector<float>::iterator p_output, int p_requested_frames) {
    if(m_state == nullptr) {
        len = std::distance(p_begin, p_end);
        len = len < p_requested_frames*m_channels ? len : p_requested_frames*m_channels;
        std::copy(p_begin, std::next(p_begin, len), p_output);
        return len;
    } else {
        len = std::distance(p_begin, p_end);

        m_data.data_in = &(*p_begin);
        m_data.data_out = &(*p_output);
        m_data.input_frames = len;
        m_data.output_frames = p_requested_frames;
        m_data.end_of_input = p_requested_frames <= len;

        if (int err = src_process(m_state, &m_data)) {
            error("resampler::process error: {}", src_strerror(err));
        }

        return m_data.input_frames_used;
    }
}

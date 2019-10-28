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
    m_float_out.swap(p_other.m_float_out);
}

resampler& resampler::operator=(resampler&& p_other) {
    m_state = p_other.m_state;
    p_other.m_state = nullptr;
    m_data = p_other.m_data;
    m_channels = p_other.m_channels;
    m_float_out.swap(p_other.m_float_out);
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

const std::vector<float>& resampler::process(std::vector<float>::const_iterator p_begin, int p_num_frames, bool p_last) {
    if(m_state == nullptr) {
        m_float_out.assign(p_begin, std::next(p_begin, p_num_frames * m_channels));
    } else {
        out_len = std::ceil(p_num_frames * m_data.src_ratio);
        m_float_out.reserve(out_len * m_channels);

        m_data.data_in = &(*p_begin);
        m_data.data_out = m_float_out.data();
        m_data.input_frames = p_num_frames;
        m_data.output_frames = out_len;
        m_data.end_of_input = p_last;

        if (int err = src_process(m_state, &m_data)) {
            error("resampler::process error: {}", src_strerror(err));
        }

        m_float_out.resize(m_data.output_frames_gen * m_channels);
    }
    return m_float_out;
}

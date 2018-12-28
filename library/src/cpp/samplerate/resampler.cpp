#include "resampler.hpp"
#include "../utility/log.hpp"

resampler::resampler(resampler::converter p_converter, int8_t p_channels, float p_ratio)
    : m_state(src_new(static_cast<int>(p_converter), p_channels, &m_error))
    , m_converter(p_converter)
    , m_channels(p_channels) {

    m_data = new SRC_DATA {
        .data_in = nullptr,
        .data_out = nullptr,
        .input_frames = 0,
        .output_frames = 0,
        .input_frames_used = 0,
        .output_frames_gen = 0,
        .end_of_input = 1,
        .src_ratio = 1.0
    };

    ratio(p_ratio);
}

resampler::resampler(const resampler& p_instance)
    : m_state(src_new(static_cast<int>(p_instance.m_converter), p_instance.m_channels, &m_error))
    , m_channels(p_instance.m_channels) {
    m_data = new SRC_DATA;
    *m_data = *p_instance.m_data;
    ratio(p_instance.m_data->src_ratio);
}

float resampler::ratio() {
    return static_cast<float>(m_data->src_ratio);
}

void resampler::check_error() {
    if (m_error != 0) {
        error("resampler error: {}", src_strerror(m_error));
        m_error = 0;
    }
}

void resampler::ratio(float p_ratio) {
    m_data->src_ratio = p_ratio;
}

resampler::~resampler() {
    src_delete(m_state);
    delete m_data;
}

std::vector<int16_t> resampler::process(std::vector<int16_t>::iterator p_begin,
                                        std::vector<int16_t>::iterator p_end,
                                        bool p_last) {
    int len = std::distance(p_begin, p_end),
        out_len = std::ceil(len * m_data->src_ratio);
    float_buf.reserve(len);
    float_out.reserve(out_len);

    src_short_to_float_array(&(*p_begin), float_buf.data(), len);

    m_data->data_in = float_buf.data();
    m_data->data_out = float_out.data();
    m_data->input_frames = len / m_channels;
    m_data->output_frames = out_len / m_channels;
    m_data->end_of_input = p_last;

    src_process(m_state, m_data);
    check_error();

    std::vector<int16_t> out(m_data->output_frames_gen * m_channels);
    src_float_to_short_array(float_out.data(), out.data(), out.capacity());
    out.resize(out.capacity());

    return out;
}

#include "audio_stream.hpp"
#include "../utility/log.hpp"

audio_stream::audio_stream(uint8_t channels, uint32_t sample_rate)
    : m_engine(oboe_engine::mode::writing, channels, sample_rate)
    , m_volume(1.0f)
{}

void audio_stream::write(std::vector<int16_t> &&pcm) {
    for (int16_t& pcm_bit : pcm) {
        pcm_bit = static_cast<int16_t>(static_cast<float>(pcm_bit) * m_volume);
    }
    m_engine.blocking_write(std::move(pcm));
}

void audio_stream::write(std::vector<float> &&pcm) {
    m_pcm_buffer.clear();
    m_pcm_buffer.reserve(pcm.size());
    std::transform(pcm.cbegin(), pcm.cend(), std::back_inserter(m_pcm_buffer),
                   [](const float &sample) {
                       auto converted = sample * std::numeric_limits<int16_t>::max();
                       return static_cast<int16_t>(converted);
                   });

    write(std::move(m_pcm_buffer));
}
#pragma once

#include "oboe_engine.hpp"
#include <type_traits>

namespace {
template<class pcm_type>
pcm_type get_normalized_pcm(pcm_type);

template <>
float get_normalized_pcm(float pcm) {
    return pcm * std::numeric_limits<int16_t>::max();
}

template <>
int16_t get_normalized_pcm(int16_t pcm) {
    return pcm;
}
}

/// oboe_engine frontend for writing PCM directly (blocking).
class audio_stream {
public:
    /// Creates an audio_stream with set channel count and sample rate.
    audio_stream(uint8_t channels, uint32_t sample_rate)
        : m_engine(oboe_engine::mode::writing, channels, sample_rate)
        , m_volume(1.0f) {
        m_engine.resume();
    }

    /// Write PCM into the stream.
    template<class iterator>
    void write(iterator begin, iterator end) {
        using pcm_type = std::remove_cv_t<std::remove_reference_t<decltype(*begin)>>;

        m_pcm_buffer.clear();
        m_pcm_buffer.reserve(std::distance(begin, end));
        std::transform(begin, end, std::back_inserter(m_pcm_buffer),
                       [this](const pcm_type& sample) {
                           pcm_type normalized = get_normalized_pcm<pcm_type>(sample);
                           float modulated = static_cast<float>(normalized) * m_volume;
                           return static_cast<int16_t>(modulated);
                       });

        m_engine.blocking_write(m_pcm_buffer.data(), m_pcm_buffer.size());
    }

    /// Set volume of audio_stream to value.
    void volume(float volume) { m_volume = std::clamp(volume, 0.0f, 1.0f); };
    /// Get volume of audio_stream.
    float volume() const { return m_volume; }

    /// Stop any audio.
    void stop() { m_engine.stop(); }
    /// Resume playing audio.
    void resume() { m_engine.resume(); }

    /// Get latency of underlying oboe stream.
    uint32_t latency() const { return m_engine.payload_size(); }
    /// Return true if this stream is mono.
    bool is_mono() const { return m_engine.is_mono(); }

private:
    oboe_engine m_engine;
    float m_volume;

    std::vector<int16_t> m_pcm_buffer;
};